package zju.tsy.client;

import java.net.Socket;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.InputStreamReader;
import java.net.InetAddress;

/**
 * 访问消息队列的客户端
 */
public class MqClient {

    private static final int SERVICE_PORT = 9999;

    // 生产消息
    public String produce(String message) throws Exception {
        // 本地的的BrokerServer.SERVICE_PORT 创建SOCKET
        Socket socket = new Socket(InetAddress.getLocalHost(), SERVICE_PORT);
        try (BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                PrintWriter out = new PrintWriter(socket.getOutputStream())) {
            out.println(message);
            out.flush();

            String result = in.readLine();
            socket.close();
            return result;
        }
    }

    // 消费消息
    public String consume() throws Exception {
        Socket socket = new Socket(InetAddress.getLocalHost(), SERVICE_PORT);
        try (BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                PrintWriter out = new PrintWriter(socket.getOutputStream())) {
            // 先向消息队列发送命令
            out.println("CONSUME");
            out.flush();

            // 再从消息队列获取一条消息
            String message = in.readLine();

            socket.close();
            return message;
        }

    }

}