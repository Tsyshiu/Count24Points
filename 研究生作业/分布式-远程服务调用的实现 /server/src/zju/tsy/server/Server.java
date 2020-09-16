package zju.tsy.server;

import java.net.ServerSocket;

public class Server {

    public static void main(String[] args) throws Exception {
        ServerSocket server = new ServerSocket(BrokerServer.SERVICE_PORT);
        JobDispatch dispatch = new JobDispatch();
        new Thread(dispatch).start();//作业调度进程
        System.out.println("远程调用已启用");
        while (true) {
            BrokerServer brokerServer = new BrokerServer(server.accept());
            System.out.println("一条远程调用请求");
            JobDispatch.messageQueue.add(brokerServer);
        }
    }
}
