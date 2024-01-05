using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Random = UnityEngine.Random;

public class CarController : MonoBehaviour
{
    private float horizontalInput;
    private float verticalInput;
    private float steerAngle;
    private bool isBreaking;

    public WheelCollider frontLeftWheelCollider;
    public WheelCollider frontRightWheelCollider;
    public WheelCollider rearLeftWheelCollider;
    public WheelCollider rearRightWheelCollider;
    public Transform frontLeftWheelTransform;
    public Transform frontRightWheelTransform;
    public Transform rearLeftWheelTransform;
    public Transform rearRightWheelTransform;

    public float maxSteeringAngle = 30f;
    public float motorForce = 1000f;
    public float brakeForce = 3000f;
    public float brake;

    public bool rwd;
    public bool fwd;
    public bool awd;

    public Vector3 initPos;
    public Vector3 initRot;
    private Rigidbody rb;

    private void Awake()
    {
        rb = GetComponent<Rigidbody>();
        if (initPos == Vector3.zero)
        {
            initPos = transform.position;
            initRot = transform.eulerAngles;
        }
    }

    public void Respawn()
    {
        // transform.localPosition = initPos;
        // transform.localPosition = new Vector3(Random.Range(-5.5f, 6.5f), -0.6f, Random.Range(-6.0f, 6.0f));
        // transform.localPosition = new Vector3(Random.Range(-7.0f, 8.0f), -0.6f, Random.Range(-2.0f, 11.0f));
        // transform.localPosition = new Vector3(1.5f, -0.6f, 0.8f);
        // transform.localPosition = new Vector3(0.7f, -0.6f, 0.8f);
        transform.localPosition = new Vector3(0, -0.6f, 0);
        transform.eulerAngles = initRot;
        rb.velocity = Vector3.zero;
        rb.angularVelocity = Vector3.zero;
    }
    
    private void FixedUpdate()
    {
        // GetInput();
        HandleMotor();
        HandleSteering();
        UpdateWheels();
    }

    public void GetInput(float hor, float ver)//, bool br)
    {
        horizontalInput = hor;
        verticalInput = ver;
        // isBreaking = br;

        // horizontalInput = Input.GetAxis("Horizontal");
        // verticalInput = Input.GetAxis("Vertical");
        // isBreaking = Input.GetKey(KeyCode.Space);
    }

    private void HandleSteering()
    {
        steerAngle = maxSteeringAngle * horizontalInput;
        frontLeftWheelCollider.steerAngle = steerAngle;
        frontRightWheelCollider.steerAngle = steerAngle;
    }

    private void HandleMotor()
    {
        if (awd)
        {
            rearLeftWheelCollider.motorTorque = verticalInput * motorForce * Time.deltaTime;
            rearRightWheelCollider.motorTorque = verticalInput * motorForce * Time.deltaTime;
            frontLeftWheelCollider.motorTorque = verticalInput * motorForce * Time.deltaTime;
            frontRightWheelCollider.motorTorque = verticalInput * motorForce * Time.deltaTime;
            rwd = false;
            fwd = false;
        }
        else if (rwd)
        {
            rearLeftWheelCollider.motorTorque = verticalInput * motorForce * Time.deltaTime;
            rearRightWheelCollider.motorTorque = verticalInput * motorForce * Time.deltaTime;
            fwd = false;
        }
        else if (fwd)
        {
            frontLeftWheelCollider.motorTorque = verticalInput * motorForce * Time.deltaTime;
            frontRightWheelCollider.motorTorque = verticalInput * motorForce * Time.deltaTime;
            rwd = false;
        }
        
        if (isBreaking)
        {
            brake = brakeForce;
        }
        else
        {
            brake = 0;
        }
        frontLeftWheelCollider.brakeTorque = brake;
        frontRightWheelCollider.brakeTorque = brake;
        rearLeftWheelCollider.brakeTorque = brake;
        rearRightWheelCollider.brakeTorque = brake;
    }

    private void UpdateWheels()
    {
        UpdateWheelPos(frontLeftWheelCollider, frontLeftWheelTransform);
        UpdateWheelPos(frontRightWheelCollider, frontRightWheelTransform);
        UpdateWheelPos(rearLeftWheelCollider, rearLeftWheelTransform);
        UpdateWheelPos(rearRightWheelCollider, rearRightWheelTransform);
    }

    private void UpdateWheelPos(WheelCollider wheelCollider, Transform trans)
    {
        Vector3 pos;
        Quaternion rot;
        wheelCollider.GetWorldPose(out pos, out rot);
        trans.rotation = rot;
        trans.position = pos;
    }

}
