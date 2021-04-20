using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System.Runtime.InteropServices;

public class gproClientPlugin
{
	[DllImport("gpro-net-Client-Plugin")]
	public static extern int foo(int bar);

	[DllImport("gpro-net-Client-Plugin")]
	public static extern int Startup();

	[DllImport("gpro-net-Client-Plugin")]
	public static extern int Shutdown();
}
