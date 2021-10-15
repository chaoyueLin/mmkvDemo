package com.example.ashmenjnidemo;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;

import androidx.annotation.Nullable;

import com.example.sharedmemlib.ISharedMem;

import java.io.IOException;

public class ShmService extends Service {
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return new SharedMemImp();
    }

    public static class SharedMemImp extends ISharedMem.Stub {
        @Override
        public ParcelFileDescriptor OpenSharedMem(String name, int size, boolean create) throws RemoteException {
            int fd =  ShmLib.OpenSharedMem(name,size,create);
            try {
                return ParcelFileDescriptor.fromFd(fd);
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }
}
