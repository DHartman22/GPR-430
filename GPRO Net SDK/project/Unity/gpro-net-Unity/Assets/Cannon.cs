using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cannon : MonoBehaviour
{
    public GameObject cannonBallPrefab;
    public float shotPower;
    public GameObject cannonModel;
    void CheckInput()
    {
        if(Input.GetKeyDown(KeyCode.Mouse0))
        {
            Fire();
        }
    }

    void Fire()
    {
        GameObject shot = Instantiate(cannonBallPrefab, transform.position, Quaternion.identity);
        //shot.transform.parent = null;
        //shot.transform.localScale = Vector3.one;
        Ray trajectory = Camera.main.ScreenPointToRay(Input.mousePosition);

        if(Physics.Raycast(trajectory, out RaycastHit hit))
        {
            Debug.DrawRay(trajectory.origin, trajectory.direction, Color.red);
            Vector3 shootDirection = hit.point - transform.position;
            shootDirection.Normalize();

            shot.GetComponent<Rigidbody>().velocity = shootDirection * shotPower;
        }
        
        
    }

    void AimCannon()
    {
        //GameObject shot = Instantiate(cannonBallPrefab, transform);
        //shot.transform.parent = null;
        //shot.transform.localScale = Vector3.one;
        Ray trajectory = Camera.main.ScreenPointToRay(Input.mousePosition);
        if (Physics.Raycast(trajectory, out RaycastHit hit))
        {
            Debug.DrawLine(trajectory.origin, trajectory.direction, Color.red);
            Vector3 shootDirection = hit.point - transform.position;
            shootDirection.Normalize();
            cannonModel.transform.LookAt(hit.point);
            //Vector3.
            //shot.GetComponent<Rigidbody>().velocity = shootDirection * shotPower;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        CheckInput();
        AimCannon();
    }
}
