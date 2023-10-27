using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AgentJumper : MonoBehaviour {

	public bool isJumping { get; private set; }
	public float distanceToTarget { get; private set; }
	public int ticksInDeadZone { get; private set; }

	public float jumpStrength { get; set; }

	private Rigidbody rb;

	// Use this for initialization
	void Start () {
		isJumping = false;
		rb = this.GetComponent<Rigidbody>();
	}

	void Update()
	{
		if(isJumping)
		{
			float difference = this.transform.position.y;
			if(difference > distanceToTarget)
			{
				distanceToTarget = difference;
			}
		}
	}

	public void performJump()
	{
		rb.AddForce(new Vector3(0, jumpStrength));
		distanceToTarget = 0.0f;
		ticksInDeadZone = 0;
	}

	void OnCollisionStay(Collision collision)
	{
		if(isJumping && collision.gameObject.CompareTag("Ground"))
		{
			isJumping = false;
		}
	}

	void OnCollisionExit(Collision collision)
	{
		if(!isJumping && collision.gameObject.CompareTag("Ground"))
		{
			isJumping = true;
		}
	}

	void OnTriggerStay(Collider collider)
	{
		if(collider.CompareTag("DeadZone"))
		{
			ticksInDeadZone++;
		}
	}
}
