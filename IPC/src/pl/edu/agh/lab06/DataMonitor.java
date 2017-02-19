package pl.edu.agh.lab06;
import java.io.IOException;
/**
 * A simple class that monitors the data and existence of a ZooKeeper
 * node. It uses asynchronous ZooKeeper APIs.
 */
import java.util.Arrays;
import java.util.List;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.AsyncCallback.StatCallback;
import org.apache.zookeeper.KeeperException.Code;
import org.apache.zookeeper.data.Stat;

import pl.edu.agh.lab06.watchers.ChildrenMonitor;

public class DataMonitor implements Watcher, StatCallback {
    ZooKeeper zk;

    String znode;

    Watcher chainedWatcher;

    boolean dead;

    DataMonitorListener listener;

    byte prevData[];
    
    Process child;

	private String[] exec;

	private ChildrenMonitor childrenMonitor;

    public DataMonitor(ZooKeeper zk, String znode, Watcher chainedWatcher,Executor listener, String[] exec) {
        this.zk = zk;
        this.znode = znode;
        this.chainedWatcher = chainedWatcher;
        this.listener = listener;
        this.exec = exec;

        try {
			Stat stat = zk.exists(znode, false);
			if(stat != null) {
				//node istnieje, uruchamiamy program
				handleProgram(false);
			}
		} catch (KeeperException e1 ) { 
			//nie ma nodea, to nic sie nie dzieje
		} catch(InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
        
        this.childrenMonitor = new ChildrenMonitor(zk, Executor.ZNODE_NAME);
        
        //rejestrujemy na zmiane
        zk.exists(znode, true, this, null);
        try {
        	zk.getChildren(znode, this);
    		zk.getChildren(znode, this.childrenMonitor);
    	} catch (KeeperException e) {
 			// jeœli nie ma nodea, to nic sie nie dzieje
 		} catch (InterruptedException e) {
 			// TODO Auto-generated catch block
 			e.printStackTrace();
 		}
    }

    public interface DataMonitorListener {
        void exists(byte data[]);

        void closing(int rc);
    }

    public void process(WatchedEvent event) {
    	String path = event.getPath();        
        if (event.getType() == Event.EventType.None) {
            switch (event.getState()) {
            case SyncConnected:
                break;
            case Expired:
                // It's all over
                dead = true;
                listener.closing(KeeperException.Code.SessionExpired);
                break;
            }
        } else if (path != null && path.equals(Executor.ZNODE_NAME)) { 
        	switch(event.getType()) {
        	case NodeCreated:
        		handleProgram(false);
        		break;
        	case NodeDeleted:
        		handleProgram(true);
        		break;
        	case NodeChildrenChanged:
        		break;
        	default: 
        		System.out.println(event.getType());
        	}
        	zk.exists(znode, true, this, null);
        	try {
        		zk.getChildren(znode, this);
        		zk.getChildren(znode, this.childrenMonitor);
        	} catch (KeeperException e) {
     			// jeœli nie ma nodea, to nic sie nie dzieje
     		} catch (InterruptedException e) {
     			// TODO Auto-generated catch block
     			e.printStackTrace();
     		}
        }
        if (chainedWatcher != null) {
            chainedWatcher.process(event);
        }
    }
    

    public void handleProgram(boolean kill) {
        if (child != null) {
            System.out.println("Killing process");
            child.destroy();
            try {
                child.waitFor();
            } catch (InterruptedException e) {
            }
            child = null;
        }
        if(!kill) {
        	try {
                System.out.println("Starting child");
                child = Runtime.getRuntime().exec(exec);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void processResult(int rc, String path, Object ctx, Stat stat) {
        boolean exists;
        switch (rc) {
        case Code.Ok:
            exists = true;
            break;
        case Code.NoNode:
            exists = false;
            break;
        case Code.SessionExpired:
        case Code.NoAuth:
            dead = true;
            listener.closing(rc);
            return;
        default:
            // Retry errors
            zk.exists(znode, true, this, null);
            return;
        }
    }
}