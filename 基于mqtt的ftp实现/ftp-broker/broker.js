"use strict";

var aedes = require("aedes")();
var server = require("net").createServer(aedes.handle);
const fs = require("fs");
const path = require("path");
const util = require("./utility");

var port = 1883;
var user_list_from_json;
var current_dir = "./ftp-main";


// override
aedes.authenticate = function (client, username, password, callback) {
    util.writeLog("authenticating ---> " + client.id);
    let error;
    // Buffer to string: String.fromCharCode.apply(null, password)
    if (username == null || password == null ||
        !isUserRegistered(username, password.toString("ascii"), user_list_from_json)) {
        error = new Error("Auth error");
        error.returnCode = 4;
    }
    callback(error, true);
};

aedes.on("clientDisconnect", function (client) {
    util.writeLog("client disconnect ---> " + client.id);
});

aedes.on("connectionError", function (client, err) {
    util.writeErr("client error " + err.message + " ---> " + client.id);
});

aedes.on("publish", function (packet, client) {
    if (client) {
        util.writeLog("message from client ---> " + client.id);
        if (packet.topic == util.topic_dataTransfer_port) {

            let payload = packet.payload;
            let count = 0;
            let idx = -1;
            for (const byte of payload) {
                ++idx;
                if (count == 8)
                    break;
                if (byte === 0)
                    ++count;
                else
                    count = 0;
            }
            var file_name = payload.toString("utf8", 0, idx - 8);

            let file_path = path.join(current_dir, file_name);
            fs.writeFile(file_path, payload.subarray(idx), err => {
                if (err) throw err;
                util.writeLog("文件已被保存至" + current_dir);
            });
        }
        else if (packet.topic == util.topic_command_port) {

            let payload = packet.payload;
            var msgNo = payload.readUInt16BE(0);
            if (msgNo >= 0x1000) // get message
                setTimeout(() => {
                    getFilesHandler(payload, msgNo, client);
                }, 0);
            else // ls
                setTimeout(() => {
                    lsHandler(payload, msgNo, client);
                }, 0);
        }
    }
    /* else a hearbeat
    MQTT has a keep alive mechanism(which is essentially a heartbeat).
    When a client connects, the keep alive value for that specific client can be specified.
    If the client is idle for one and a half the time specified and 
    does not send any heartbeat or any other MQTT message, 
    the broker assumes the TCP connection is broken(= half - open). */
});

aedes.on("subscribe", function (subscriptions, client) {
    if (client) {
        util.writeLog("subscribe from client, topic: " + subscriptions[0].topic + " ---> " + client.id);
    }
});

aedes.on("client", function (client) {
    util.writeLog("new client ---> " + client.id);
});


/**
 * @param username {String}
 * @param password {String}
 * @param user_list {Array}
 * @description 检查用户是否在user-list中
 */
function isUserRegistered(username, password, user_list) {
    return user_list.some(one_user => {
        if (one_user.username == username && one_user.password == password)
            return true;
    });
}

/**
 * @param payload {Buffer}
 * @param msgNo {Number}
 * @param client 
 */
function lsHandler(payload, msgNo, client) {
    let command = payload.toString("utf8", 2).trim().split(new RegExp("\\s+"));
    if (command[0] == "ls" || command[0] == "dir") {
        let ls_path = command.length == 2 ? path.join(current_dir, command[1]) : current_dir;
        var ls_result = fs.readdirSync(ls_path);
        var data = Buffer.from(ls_result.join("\t"));
        var msg_head = Buffer.allocUnsafe(2);
        msg_head.writeUInt16BE(msgNo, 0);
        data = Buffer.concat([msg_head, data]);
        let packet = {
            cmd: 'publish',
            messageId: 42,
            qos: 2,
            dup: false,
            topic: util.topic_command_port,
            payload: data,
            retain: false
        };
        client.publish(packet, null);
    }
    else
        util.writeErr("ls command error");
}

/**
 * @param payload {Buffer}
 * @param msgNo {Number}
 * @param client 
 */
function getFilesHandler(payload, msgNo, client) {
    let file_count = payload.readUInt16BE(2);
    let index = 7;
    let files_to_get = payload.toString("utf8", 7).trim().split(new RegExp("\\s+"));
    for (const file_path of files_to_get) {
        try {
            var data = fs.readFileSync(path.join(current_dir, file_path));
            var msg_head = Buffer.allocUnsafe(2);
            msg_head.writeUInt16BE(msgNo++, 0);
            data = Buffer.concat([msg_head, data]);
        } catch (err) {
            if (err.code === "ENOENT") {
                util.writeErr(file_path + " 不存在");
                return;
            }
        }

        let packet = {
            cmd: 'publish',
            messageId: 42,
            qos: 2,
            dup: false,
            topic: util.topic_dataTransfer_port,
            payload: data,
            retain: false
        };
        client.publish(packet, null);
    }

}


(function main() {
    try {
        let data = fs.readFileSync("user-list.json");
        user_list_from_json = JSON.parse(data.toString());

        server.listen(port, function () {
            util.writeLog("ftp server listening on port " + port);
        });
    } catch (err) {
        if (err.code === "ENOENT") {
            util.writeErr("user-list.json 不存在");
            return;
        }
    }
})();
