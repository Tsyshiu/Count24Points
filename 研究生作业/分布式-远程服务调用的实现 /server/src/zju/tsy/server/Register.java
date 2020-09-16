package zju.tsy.server;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.net.Socket;

/**
 * Register注册服务商
 */
public class Register {

    public static Map<Integer, Socket> providerList = new HashMap<Integer, Socket>();

    /**
     * 往providerList添加服务提供商
     * @param providerID {int} 
     * @param socket {Socket} 服务提供商的socket连接
     */
    public static synchronized void add(int providerID, Socket socket) {
        providerList.put(providerID, socket);
    }

    /**
     * 检查某个服务提供商是否可用
     */
    public static synchronized boolean isAvailable(int providerID) {

        return providerList.containsKey(providerID) && !providerList.get(providerID).isClosed();
    }

    /**
     * 删除所有服务提供商
     */
    public static synchronized void clear() {
        providerList.clear();
    }
}