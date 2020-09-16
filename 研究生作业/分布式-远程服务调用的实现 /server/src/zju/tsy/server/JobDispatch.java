package zju.tsy.server;

import java.util.concurrent.ArrayBlockingQueue;

/**
 * JobDispatch: FIFO管理作业
 */
public class JobDispatch implements Runnable {

    public static final int MAX_JOBS = 4;

    // 作业队列
    public static ArrayBlockingQueue<BrokerServer> messageQueue = new ArrayBlockingQueue<BrokerServer>(10);

    // 
    public static int currentJobCount = 0;

    @Override
    public void run() {
        try {
            while (true) {
                if (currentJobCount < MAX_JOBS && !messageQueue.isEmpty()) {
                    BrokerServer bServer = messageQueue.poll();
                    new Thread(bServer).start();
                    currentJobCount++;
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}