using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class gproClientManager : MonoBehaviour
{
    public InputField loginInput;
    public GameObject player;
    // Start is called before the first frame update
    void Start()
    {
        player = GameObject.FindGameObjectWithTag("Player");
    }

    public void Login()
    {
        Debug.Log(gameObject.name);

        //char name = (char)loginInput.text;
        //int test = gproClientPlugin.Login();
        //test = gproClientPlugin.Login();
        bool a = gproClientPlugin.StartClient();
        Debug.Log(a);        
        //GameObject.FindGameObjectWithTag("Player").GetComponent<Text>().text = gproClientPlugin.GetUsername().ToString();
        


    }

    void CheckInput()
    {
        if(Input.GetKeyDown(KeyCode.P))
        {
            int test = gproClientPlugin.Shutdown();
            Debug.Log(test);
        }
    }

    // Update is called once per frame
    void Update()
    {

        if (gproClientPlugin.IsActive())
        {
            gproClientPlugin.MessageLoop();
            CheckInput();
            char[] a = player.transform.position.x.ToString().ToCharArray();
            
            Debug.Log(gproClientPlugin.UpdatePlayerState(player.transform.position.x,
            player.transform.position.y, player.transform.position.z, a));
            
        }
    }
}
