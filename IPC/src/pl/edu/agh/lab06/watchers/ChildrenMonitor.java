package pl.edu.agh.lab06.watchers;

import java.util.List;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.Watcher.Event;

import pl.edu.agh.lab06.Executor;

public class ChildrenMonitor implements Watcher {
	private ZooKeeper zk;
	private String path;
	private int lastNumberOfChildren = 0;
	private int currentNumberOfChildren = 0;

	public ChildrenMonitor(ZooKeeper zk, String path) {
		this.zk = zk;
		this.path = path;
		
		//zacznij od sprawdzenia czy nie ma dzieci istniejacych
		try {
			List<String> children = zk.getChildren(this.path, false);
			lastNumberOfChildren = children.size();
			currentNumberOfChildren  = lastNumberOfChildren;
			for(String child : children) {
				String childZnode = path + "/" + child;
				ChildrenMonitor childrenMonitor = new ChildrenMonitor(zk, childZnode);
        		zk.getChildren(childZnode, childrenMonitor);
        		zk.getChildren(this.path, this);
			}
		} catch (KeeperException e) { 
			//node nie istnieje
		} catch(InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	@Override
	public void process(WatchedEvent event) {
		String path = event.getPath();
        
        if (event.getType() == Event.EventType.None) {
        	switch (event.getState()) {
            case SyncConnected:
                break;
            case Expired:
                //dead = true;
                //listener.closing(KeeperException.Code.SessionExpired);
                break;
            }
        } else if (path != null) {
        	switch(event.getType()) {        	
        	case NodeCreated:
        		break;
        	case NodeDeleted:
        		break;
        	case NodeChildrenChanged:
    			handleChildrenChange();
        		break;
        	}
        	showChildrenCount();
        }
	}
	
	private void handleChildrenChange() {
		try {
			List<String> children = zk.getChildren(this.path, false);
			lastNumberOfChildren = currentNumberOfChildren ;
			currentNumberOfChildren  = children.size();
			for(String child : children) {
				String childZnode = path + "/" + child;
				ChildrenMonitor childrenMonitor = new ChildrenMonitor(zk, Executor.ZNODE_NAME);
        		zk.getChildren(childZnode, childrenMonitor);
        		zk.getChildren(this.path, this);
			}
			
			zk.getChildren(path, this);
		} catch (KeeperException | InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private void showChildrenCount() {
		if(currentNumberOfChildren != lastNumberOfChildren) {
			System.out.println("Liczba potomkow " + Executor.ZNODE_NAME + " : " + deepDescendntCount(Executor.ZNODE_NAME));
		}
	}

    private int deepDescendntCount(String znode) {
    	int counter = 0;
    	try {
			List<String> children = zk.getChildren(znode, false);
			for(String child : children) {
				String childZnode = znode + "/" + child;
				System.out.println(childZnode);
				counter += deepDescendntCount(childZnode);
			}
			counter += children.size();
		} catch (KeeperException e) { 
			
		} catch(InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    	return counter;
    }
}
