using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// Could make a Coroutine Node that FailureLeafNode and PrintDebugLeafNode inherit from

public class FailureLeafNode : TreeNode
{
    [SerializeField] float waitTime = 5f;
    private float elapsedTime = 0f;

    protected override void NodeFunction()
    {
        elapsedTime += Time.deltaTime;
        if(elapsedTime > waitTime)
        {
            elapsedTime = 0f;
            this.state = State.FAILURE;
        }
        
    }

    public override void ResetNode()
    {
        // If this not has a state change forced, cancel the coroutine
        base.ResetNode();

        elapsedTime = 0f;
    }
}
