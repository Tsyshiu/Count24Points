package zju.tsy.client;

/**
 * Client
 */
public class Client {

    public static void main(String[] args) {
        try {
            MqClient client = new MqClient();

            String  message = client.produce("CALL:1:helloWorld");
            System.out.println("return：" + message);
            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}