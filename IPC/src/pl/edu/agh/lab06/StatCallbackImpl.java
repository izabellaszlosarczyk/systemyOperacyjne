package pl.edu.agh.lab06;
import java.util.Arrays;

import org.apache.zookeeper.AsyncCallback.StatCallback;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.KeeperException.Code;
import org.apache.zookeeper.data.Stat;

public class StatCallbackImpl implements StatCallback {

    public void processResult(int rc, String path, Object ctx, Stat stat) {
    }
}
