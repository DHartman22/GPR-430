using System.Collections;
using System.Collections.Generic;
using UnityEngine;
    public class PlayerMovement : MonoBehaviour
    {
        public float xMove = 0f;
        public float zMove = 0f;
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
        public bool inputAllowed;
        public bool gravityPaused = false;

        public Camera playerCamera;

        private Vector3 right;
        private Vector3 forward;

        bool spaceHeld;

        void CheckInput()
        {
            xMove = Input.GetAxis("Horizontal");

            zMove = Input.GetAxis("Vertical");

            spaceHeld = Input.GetKey(KeyCode.Space);
        }

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

                

                movement = right * xMove + forward * zMove;

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

        void Jump()
        {
            if (spaceHeld && isGrounded)
            {
                velocity.y = Mathf.Sqrt(jumpHeight * -2 * gravity);
            }
        }

        void JumpNoInput()
        {
            if(isGrounded)
            {
                velocity.y = Mathf.Sqrt(jumpHeight * -2 * gravity);
            }
        }
        // Start is called before the first frame update
        void Start()
        {
        right = transform.right;
        forward = transform.forward;
        }

        // Update is called once per frame
        private void Update()
        {
            if(inputAllowed)
            {
                CheckInput();
            }
        }

        void FixedUpdate()
        {
            if (inputAllowed)
            {
                //Jump();
                Move();
                //Jump();
                //if(Input.GetKey(KeyCode.Space))
                //{
                //    CmdPlayerMove();
                //}
            }
        }

        //[Command]
        //private void CmdPlayerMove()
        //{
        //    RpcPlayerMove();
        //}

        //[ClientRpc]
        //private void RpcPlayerMove()
        //{
        //        Move();
        //        Jump();
        //    //JumpNoInput();
        //    //transform.Translate(new Vector3(1, 1, 2));
        //}

        //private void FixedUpdate()
        //{
        //    if(isLocalPlayer)
        //    {
        //        Move();
        //    }
        //}

    }
