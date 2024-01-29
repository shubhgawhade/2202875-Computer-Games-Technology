using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// Could make a Coroutine Node that FailureLeafNode and PrintDebugLeafNode inherit from

public class PrintDebugLeafNode : TreeNode
{
    [SerializeField] string message = "";
    [SerializeField] float waitTime = 5f;
    private float elapsedTime = 0f;

    protected override void NodeFunction()
    {
        // Check if the coroutine is running, if not, run it
        elapsedTime += Time.deltaTime;
        if(elapsedTime > waitTime)
        {
            Debug.Log(message);
            elapsedTime = 0f;
            this.state = State.SUCCESS;
        }
    }

    public override void ResetNode()
    {
        // If this not has a state change forced, cancel the coroutine
        base.ResetNode();

        elapsedTime = 0f;
    }
}
