using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMovement : MonoBehaviour
{
    float xMove = 0f;
    float zMove = 0f;
    Vector3 movement;
    public float speed = 15f;
    public float runSpeedBoost = 3f;
    public float stompWindow; //Meant to prevent stomps while rising
    public float stompVelocity;
    public CharacterController characterController;

    //gravity
    public Vector3 velocity;
    public float gravity = -9.8f;
    public Transform groundCheck;
    public Transform bonkCheck;
    bool isGrounded;
    public LayerMask groundMask;
    public LayerMask bouncyMask;
    float groundCheckDistance = 0.5f;

    public float jumpHeight = 5f;
    public bool inputAllowed = true;
    public bool gravityPaused = false;

    private AudioSource bounce;

    void Move()
    {
        if (inputAllowed)
        {
            isGrounded = Physics.CheckSphere(groundCheck.position, groundCheckDistance, groundMask);
            if (isGrounded && velocity.y < 0)
            {
                velocity.y = -1;
            }

            if (gravityPaused == false)
            {
                velocity.y += gravity * Time.deltaTime;
                characterController.Move(velocity * Time.deltaTime);
            }

            xMove = Input.GetAxis("Horizontal");

            zMove = Input.GetAxis("Vertical");

            movement = transform.right * xMove + transform.forward * zMove;

            if (Input.GetKey(KeyCode.LeftShift))
            {
                characterController.Move(movement * (speed + runSpeedBoost) * Time.deltaTime);
            }
            else
            {
                characterController.Move(movement * speed * Time.deltaTime);
            }
        }
    }

    private void OnControllerColliderHit(ControllerColliderHit hit)
    {
        if (hit.gameObject.tag == "Trampoline")
        {
            Bounce();
        }
    }

    void Jump()
    {
        if (Input.GetButton("Jump") && isGrounded)
        {
            velocity.y = Mathf.Sqrt(jumpHeight * -2 * gravity);
        }
    }

    public void Bounce()
    {
        if (Physics.CheckSphere(groundCheck.position, groundCheckDistance, bouncyMask))
        {
            if (velocity.y > (Mathf.Sqrt((jumpHeight * 1.5f) * -2 * gravity) * -1) - 0.1f) //Prevents idle bouncing from triggering the bigger bounce effects 
            {
                velocity.y = Mathf.Sqrt((jumpHeight * 1.5f) * -2 * gravity);
                bounce.pitch = 1;
                bounce.Play();
            }
            else
            {
                velocity.y *= -1;
                bounce.pitch = 2;
                bounce.Play();
            }
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        bounce = GetComponent<AudioSource>();
    }

    // Update is called once per frame
    void Update()
    {
        if (inputAllowed)
        {
            Jump();
        }
    }

    private void FixedUpdate()
    {
        Move();
    }
}
