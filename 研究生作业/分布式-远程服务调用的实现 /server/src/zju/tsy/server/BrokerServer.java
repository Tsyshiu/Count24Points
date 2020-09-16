package zju.tsy.server;

import java.net.Socket;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.InputStreamReader;

/**
 * 用于启动消息处理中心
 */
public class BrokerServer implements Runnable {

    public static final int SERVICE_PORT = 9999;

    private final Socket socket;

    public BrokerServer(Socket socket) {
        this.socket = socket;
    }

    @Override
    public void run() {
        try (BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                PrintWriter out = new PrintWriter(socket.getOutputStream())) {

            String str = in.readLine();

            System.out.println("接收到原始数据：" + str);
            if (str == null) {
                // do nothing;
            } else if (str.equals("CONSUME")) { // CONSUME 表示要消费一条消息
                // 从消息队列中消费一条消息
                String message = Broker.consume();
                out.println(message);
                out.flush();
            } else if (str.startsWith("SEND:")) {
                // 接受到的请求包含SEND:字符串 表示生产消息放到消息队列中
                Broker.produce(str);
            } else if (str.startsWith("CALL:")) {

                // 获取方法
                String splitted[] = str.split(":");
                int providerID = Integer.parseInt(splitted[1]);

                if (Register.isAvailable(providerID)) {
                    Socket s = Register.providerList.get(providerID);

                    PrintWriter printWriter = new PrintWriter(s.getOutputStream());
                    printWriter.println(str);
                    printWriter.flush();
                    
 
                    InputStreamReader input = new InputStreamReader(s.getInputStream());

                    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(s.getInputStream()));
                    String result = bufferedReader.readLine();

                    out.println(result);
                    out.flush();


                } else {
                    out.println("warning: service unavailable");
                    out.flush();
                }
                JobDispatch.currentJobCount--;

            } else if (str.startsWith("REGISTER:")) {

                Register.add(Integer.parseInt(str.substring(9)),
                        new Socket(socket.getInetAddress().getHostAddress(), socket.getPort()));

            } else {
                System.out.println("原始数据:" + str + "没有遵循协议,不提供相关服务");
            }
            socket.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}