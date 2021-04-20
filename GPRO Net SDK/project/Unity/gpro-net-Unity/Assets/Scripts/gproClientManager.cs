using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class gproClientManager : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
		int test = gproClientPlugin.foo(9000);
		Debug.Log(test);
        GameObject.FindGameObjectWithTag("Player").GetComponent<Text>().text = test.ToString();
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
