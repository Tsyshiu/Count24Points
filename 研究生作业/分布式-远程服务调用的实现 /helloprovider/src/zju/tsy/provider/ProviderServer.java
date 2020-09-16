package zju.tsy.provider;

import java.net.Socket;
import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.InputStreamReader;
import java.lang.reflect.Method;

/**
 * Broker的消息处理中心
 */
public class ProviderServer implements Runnable {

    public static int SERVICE_PORT = 1234;
    private static String APIInterfaceName = "";

    private final Socket socket;

    public ProviderServer(Socket socket) {
        this.socket = socket;
    }

    public static void setAPIInterfaceName(String name) {
        APIInterfaceName = name;
    }

    @Override
    public void run() {
        try (BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                PrintWriter out = new PrintWriter(socket.getOutputStream())) {
            while (true) {
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
                    Class<?> apiClass = Class.forName(APIInterfaceName + "Impl");
                    Object api = apiClass.newInstance();

                    // 获取方法
                    String splitted[] = str.split(":");
                    String methodName = splitted[2];
                    java.util.List<String> parameters = new ArrayList<String>();
                    parameters.clear();
                    for (int i = 3; i < splitted.length; i++) {
                        parameters.add(splitted[i]);
                    }

                    Method m = api.getClass().getDeclaredMethod(methodName);

                    // 调用方法
                    String result = m.getParameterCount() == 0 ? (String) m.invoke(api)
                            : (String) m.invoke(api, parameters);

                    out.println(result);
                    out.flush();
                } else {
                    System.out.println("原始数据:" + str + "没有遵循协议,不提供相关服务");
                }
            }

        } catch (ClassNotFoundException e) {
            System.err.println("找不到远程调用类!!!");
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}