package zju.tsy.helloworld;

import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class HelloWorldAPIImpl extends UnicastRemoteObject implements HelloWorldAPI {

    private static final long serialVersionUID = 9205883452934019060L;

    public HelloWorldAPIImpl() throws RemoteException {
    }

    @Override
    public String helloWorld() {
        return "Hello world!";
    }

}
