package pl.edu.agh.lab06;
import java.util.Scanner;

public class Main {
	public static void main(String[] args) {
        if (args.length < 2) {
            System.err
                    .println("Uzycie: adressZookeepera program [argumenty ...]");
            System.exit(2);
        }
        String hostPort = args[0];
        String znode = Executor.ZNODE_NAME;
        String filename = "output.txt"; //TODO: tymczasowe
        String exec[] = new String[args.length - 1];
        System.arraycopy(args, 1, exec, 0, exec.length);

        Thread executorThread = null;
        Executor executor = null; 
        try {
        	executor = new Executor(hostPort, znode, filename, exec);
            executorThread = new Thread(executor);
            executorThread.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.println("Zookeeper - watcher"); 
        System.out.println("Adress: " + hostPort);
        System.out.println("Program: " + args[1]);
       
		Scanner scanner = new Scanner(System.in);
		String input = "";
		while (!input.contains("exit")) {
			System.out.println("Menu:");
			System.out.println("[tree] - wyswietl drzewo");
			System.out.println("[exit] - wyjdz");
			
			if(scanner.hasNext()) {
				input = scanner.next();
			}
			
			if(input.contains("tree")) {
				executor.deepLs(Executor.ZNODE_NAME);
			}
		}
		System.out.println("Koncze programm...");
		executorThread.stop();
	}
}
