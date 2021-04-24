using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System.Runtime.InteropServices;
using System;

public class gproClientPlugin
{
	[DllImport("gpro-net-Client-Plugin")]
	public static extern int foo(int bar);

	[DllImport("gpro-net-Client-Plugin")]
	public static extern int Startup();

	[DllImport("gpro-net-Client-Plugin")]
	public static extern int Shutdown();

	[DllImport("gpro-net-Client-Plugin")]
	public static extern int MessageLoop();

	[DllImport("gpro-net-Client-Plugin")]
	public static extern int Login();

	[DllImport("gpro-net-Client-Plugin")]
	public static extern char[] GetUsername();

	[DllImport("gpro-net-Client-Plugin")]
	public static extern bool StartClient();

	[DllImport("gpro-net-Client-Plugin")]
	public static extern bool IsActive();

	[DllImport("gpro-net-Client-Plugin")]
	public static extern char[] UpdatePlayerState(float x, float y, float z, char[] test);

}
