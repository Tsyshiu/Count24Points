package zju.tsy.provider;

import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;


/**
 * api提供者---一个api服务提供商的样例
 * api服务提供商只需把HelloWorldAPI接口文件发布给客户，客户即可使用
 * 此处以helloworldAPI为例
 */
public class Provider {

    public static void main(String[] args) throws Exception {

        // 设置提供的api为HelloWorldAPI
        ProviderServer.setAPIInterfaceName("zju.tsy.helloworld.HelloWorldAPI");

        // 注册服务
        String host = "192.168.0.109";
        int port = 9999;
        // 与服务端建立连接
        Socket socket = new Socket(host, port);
        ProviderServer.SERVICE_PORT = socket.getLocalPort();

        // 建立连接后获得输出流
        String message = "REGISTER:1";
        try (PrintWriter out = new PrintWriter(socket.getOutputStream())) {
            out.println(message);
            out.flush();
        }
        socket.close();
        // 通过shutdownOutput高速服务器已经发送完数据，后续只能接受数据
        // socket.shutdownOutput();

        ServerSocket server = new ServerSocket(ProviderServer.SERVICE_PORT);

        System.out.println("Helloworld服务已启用");
        while (true) {
            ProviderServer brokerServer = new ProviderServer(server.accept());
            System.out.println("成功接入服务注册中心，服务可远程访问");
            new Thread(brokerServer).start();
        }
    }
}
