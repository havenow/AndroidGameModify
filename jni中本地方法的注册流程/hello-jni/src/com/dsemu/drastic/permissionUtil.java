package com.dsemu.drastic;

import android.app.Activity;
import android.content.pm.PackageManager;

public class permissionUtil {
	private static final int REQUEST_EXTERNAL_STORAGE = 1;

	private static String[] PERMISSIONS_STORAGE = {
		"android.permission.READ_EXTERNAL_STORAGE",
		"android.permission.WRITE_EXTERNAL_STORAGE" 
	};

	  public static void verifyStoragePermissions(Activity activity)
	  {
//		  try {
//			  //int storagePermission = activity.checkSelfPermission("android.permission.WRITE_EXTERNAL_STORAGE");
//			  int storagePermission = activity.checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
//			  if (storagePermission != PackageManager.PERMISSION_GRANTED) {
//				  activity.requestPermissions(PERMISSIONS_STORAGE, REQUEST_EXTERNAL_STORAGE);
//				  ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_EXTERNAL_STORAGE_REQUEST_CODE);
//			  }
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
	  }

}
