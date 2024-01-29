using System;
using System.Collections.Generic;
using Unity.MLAgents;
using Unity.MLAgents.Actuators;
using Unity.MLAgents.Sensors;
using UnityEngine;
using Random = UnityEngine.Random;

public class CarAgent : Agent
{
    [SerializeField] private Transform[] targets;
    [SerializeField] private GameObject targetObj;
    [SerializeField] private GameObject prevTargetObj;
    
    private CarController _carController;

    private Vector3 distance;

    public float Distance;
    public float closestPoint = 1000;
    public GameObject closestCheckpoint;

    private Rigidbody rb;
    
    public float timer;
    public float killTimer;
    
    private RayPerceptionOutput _rayPerceptionOutput;
    [SerializeField] private GameObject rayObject;
    
    public int score;
    
    private void Awake()
    {
        _carController = GetComponent<CarController>();
        rb = GetComponent<Rigidbody>();
    }

    private void OnDrawGizmos()
    {
        if (targetObj)
        {
            Gizmos.DrawLine(transform.position, targetObj.transform.position);
        }
    }

    RaycastHit hit;
    private void FixedUpdate()
    {
        // AddReward(-0.0001f);
        AddReward(0.0001f);

        distance = transform.localPosition - targetObj.transform.localPosition;
        Distance = distance.magnitude;

        // if (Vector3.Dot(transform.forward, -distance.normalized) > 0.4f)
        // {
        //     // print("DISTANCE");
        //     AddReward(-0.00001f * Distance);
        // }
        // else 
        if (closestCheckpoint != null)
        {
            // print((transform.localPosition - closestCheckpoint.transform.localPosition).magnitude);
            AddReward(-0.0001f * (transform.localPosition - closestCheckpoint.transform.localPosition).magnitude);
        }

        if (transform.position.y < -4f)
        {
            // print(rb.velocity.y);
            AddReward(-1f);
            EndEpisode();
        }

        if (rb.velocity.magnitude < 0.3f && rb.velocity.magnitude > -0.3f)
        {
            timer += Time.fixedDeltaTime;
        }
        else
        {
            timer = 0;
        }
        
        if (timer > 5)
        {
            timer = 0;
            AddReward(-1f);
            EndEpisode();
        }

        killTimer += Time.deltaTime;
    }

    //Called each time it has timed-out or has reached the goal
    public override void OnEpisodeBegin()
    { 
        // target.transform.localPosition = new Vector3(Random.Range(-7.0f, 8.0f), -0.3f, Random.Range(-2.0f, 11.0f));
        
        // if (checkpoints >= 67)
        // {
        //     print($"CP: {checkpoints}");
        // }
        
        checkpoints = 0;
        checkpointsCollected = new List<Collider>();
        _carController.Respawn();
        
        killTimer = 0;
        score = 0;
        
        SetTarget();
    }

    private void SetTarget()
    {
        // SetReward(0);

        checkpointsCollected = new List<Collider>();
        
        foreach (Transform target in targets)
        {
            target.gameObject.SetActive(false);
        }

        do
        {
            targetObj = targets[Random.Range(0, targets.Length)].gameObject;
        } while (targetObj == prevTargetObj);
        
        prevTargetObj = targetObj;


        // targetObj = targets[score % targets.Length].gameObject;
        
        // do
        // {
        //     targetObj = targets[Random.Range(0, targets.Length)].gameObject;
        //     
        // } while ((targetObj.transform.localPosition - transform.localPosition).magnitude < 1.0f);
        
        // int overlapObjects = -1;
        // do
        // {
        //     targetObj.transform.localPosition = new Vector3(Random.Range(-8, 8), -0.63f,
        //         Random.Range(-8, 8));
        //     
        //     Collider[] col = Physics.OverlapBox(targetObj.transform.localPosition, 
        //         targetObj.transform.localScale / 2, quaternion.identity);
        //     
        //     overlapObjects = col.Length;
        // } while (overlapObjects > 1);

        targetObj.SetActive(true);
        closestPoint = 1000;
    }

    //Collecting extra Information that isn't picked up by the RaycastSensors
    public override void CollectObservations(VectorSensor sensor)
    {
        sensor.AddObservation(transform.localPosition);
        sensor.AddObservation(targetObj.transform.localPosition);
        sensor.AddObservation(score);
        sensor.AddObservation(checkpoints);
        // sensor.AddObservation(checkpointsCollected.Count);
    }

    //Processing the actions received
    public override void OnActionReceived(ActionBuffers actions)
    {
        ActionSegment<float> continuousActions = actions.ContinuousActions;
       
        if (continuousActions[1] > 0)
        {
            AddReward(0.001f);
        }
        
        if (continuousActions[1] < 0)
        {
            AddReward(-0.01f);
        }
        
        _carController.GetInput(continuousActions[0], continuousActions[1]); //, temp);

        RayPerceptionSensorComponent3D[] sensors = rayObject.GetComponents<RayPerceptionSensorComponent3D>();
        foreach (RayPerceptionSensorComponent3D sensor in sensors)
        {
            _rayPerceptionOutput = sensor.RaySensor.RayPerceptionOutput;
            if (sensor.SensorName == "Front")
            {
                if (_rayPerceptionOutput.RayOutputs != null)
                {
                    foreach (RayPerceptionOutput.RayOutput output in _rayPerceptionOutput.RayOutputs)
                    {
                        if (output.HasHit && output.HitGameObject.CompareTag("Checkpoint"))
                        {
                            foreach (Collider checkpoint in checkpointsCollected)
                            {
                                if (checkpoint.gameObject == output.HitGameObject)
                                {
                                    return;
                                }
                            }
                            
                            float distFromCheckpointToTarget = (output.HitGameObject.transform.localPosition -
                                                                targetObj.transform.localPosition).magnitude;
                            
                            if (output.HitGameObject == targetObj)
                            {
                                closestCheckpoint = output.HitGameObject;
                                closestPoint = (output.HitGameObject.transform.position - targetObj.transform.localPosition)
                                    .magnitude;
                                
                                AddReward(0.000001f / output.HitFraction);
                                break;
                            }

                            if ((distFromCheckpointToTarget <= Distance || distFromCheckpointToTarget <= closestPoint
                                                                        || Math.Abs(Distance -
                                                                            distFromCheckpointToTarget) < 0.6f)
                                && Vector3.Dot(output.HitGameObject.transform.forward, transform.forward) > 0.0f)
                                // && Math.Abs(Distance - distFromCheckpointToTarget) < 0.6f)
                                // && Vector3.Dot(output.HitGameObject.transform.forward, transform.forward) > 0.0f)
                            {
                                closestCheckpoint = output.HitGameObject;
                                closestPoint = (output.HitGameObject.transform.position - targetObj.transform.localPosition)
                                    .magnitude;

                                AddReward(-0.000001f * output.HitFraction);
                            }
                        }
                    }
                }
            }
        }
    }

    //For manual testing with human input, the actionsOut defined here will be sent to OnActionReceived
    public override void Heuristic(in ActionBuffers actionsOut)
    {
        ActionSegment<float> continuousActions = actionsOut.ContinuousActions;

        continuousActions[0] = Input.GetAxis("Horizontal");
        continuousActions[1] = Input.GetAxis("Vertical");
    }

    private void OnCollisionEnter(Collision other)
    {
        if (other.collider.CompareTag("Player") || other.collider.CompareTag("Obstacle"))
        {
            if (killTimer > 1f)
            {
                print($"CRASH! {other.gameObject.name}");
                AddReward(-1f);
            }
            
            EndEpisode();
        }
    }

    public int checkpoints;
    public List<Collider> checkpointsCollected = new List<Collider>();
    private void OnTriggerEnter(Collider other)
    {
        foreach (Collider checkpoint in checkpointsCollected)
        {
            if (checkpoint.gameObject == other.gameObject)
            {
                AddReward(-1.0f);
                EndEpisode();
                return;
            }
        }
        
        if (other.CompareTag("Checkpoint") &&
            Vector3.Dot(other.gameObject.transform.forward, transform.forward) > 0.0f)
        {
            checkpointsCollected.Add(other);
            checkpoints++;
            
            if (other.gameObject == targetObj)
            {
                print($"REACHED TARGET {score}!!!");
                score++;
                AddReward(score / 5.0f);
                SetTarget();
            }
            
            if(other.gameObject == closestCheckpoint)
            {
                AddReward(0.05f);
            }
            else
            {
                AddReward(-0.01f);
            }
        }
        else
        {
            checkpointsCollected.Add(other);
            AddReward(-1.0f);
            EndEpisode();
        }
    }
}