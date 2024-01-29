using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Decorator : SelectorNode
{
    [SerializeField] float runTime = 5f;
    float elapsedTime = 0f;
    protected override void NodeFunction()
    {
        // Update elapsed time
        elapsedTime += Time.deltaTime;

        // If the node is allowed to run
        if (elapsedTime < runTime)
        {
            // Get first child node (assume others don't exist)
            TreeNode child = childNodes[0];

            // Start Node - This does nothing if its already running
            child.StartNode();

            // Run the Node and get its state
            State childState = child.RunNode();

            // If the child node stops running, reset the timer and update the decorator with the child nodes state
            if(childState != State.RUNNING)
            {
                this.state = childState;
                elapsedTime = 0f;
            }
        }
        else
        {
            // If we run out of time, assume node is successful and force this state into the child node
            this.state = State.SUCCESS;
            childNodes[0].ForceStateChange(State.SUCCESS);
            elapsedTime = 0f;
        }
    }
}