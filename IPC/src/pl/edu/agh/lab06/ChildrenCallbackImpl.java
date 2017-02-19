package pl.edu.agh.lab06;
import java.util.List;

import org.apache.zookeeper.AsyncCallback.ChildrenCallback;
import org.apache.zookeeper.ZooKeeper;

public class ChildrenCallbackImpl implements ChildrenCallback {

	private ZooKeeper zk;
	private String znode;
	private DataMonitor dataMonitor;

	public ChildrenCallbackImpl(ZooKeeper zk, String znode, DataMonitor dataMonitor) {
		this.zk = zk;
		this.znode = znode;
		this.dataMonitor = dataMonitor;
	}

	@Override
	public void processResult(int rc, String path, Object ctx, List<String> children) {
		// TODO Auto-generated method stub
		System.out.println("rc: " + rc);
		System.out.println("path: " + path);
		for(String child : children) {
			System.out.println(child);
		}
		System.out.println("Liczba potomkow /znode_testowy: " + children.size());
	}

}
