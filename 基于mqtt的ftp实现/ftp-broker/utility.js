"use strict";

module.exports = {
    topic_dataTransfer_port: "dataTransfer",
    topic_command_port: "command",
    
    writeLog: writeLog,
    writeErr: writeErr
}

/**
 * @param log {String}
 */
function writeLog(log) {
    console.log(new Date().toLocaleString() + ": " + log);
}

/**
 * @param ErrMessage {String}
 */
function writeErr(ErrMessage) {
    console.log(new Date().toLocaleString() + " ERR: " + ErrMessage);
}