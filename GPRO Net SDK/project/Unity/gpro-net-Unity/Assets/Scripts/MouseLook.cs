﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MouseLook : MonoBehaviour
{
    public float mouseX = 0f;
    public float mouseY = 0f;
    public float mouseSpeed = 100f;
    public Transform player;

    float xRot = 0f;

    void Look()
    {
        //rotating the players body left and right
        mouseX = Input.GetAxis("Mouse X") * mouseSpeed * Time.deltaTime;
        player.Rotate(Vector3.up, mouseX);
        //Debug.Log(Vector3.right * mouseX);

        //rotating the camera up and down
        mouseY = Input.GetAxis("Mouse Y") * mouseSpeed * Time.deltaTime;
        xRot -= mouseY;
        xRot = Mathf.Clamp(xRot, -90f, 90f);
        transform.localRotation = Quaternion.Euler(xRot, 0f, 0f);
        Rigidbody rgd;
    }

    // Start is called before the first frame update
    void Start()
    {
        Cursor.lockState = CursorLockMode.Locked;
    }

    // Update is called once per frame
    void Update()
    {
        Look();
    }
}
