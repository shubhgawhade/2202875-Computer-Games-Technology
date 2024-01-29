using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TreeNode : MonoBehaviour
{
    // Enum describing available states of the node
    public enum State { WAITING, RUNNING, SUCCESS, FAILURE };

    // Current state of this specific node
    protected State state = State.WAITING;

    void Update()
    {
        // Grab the rendered and update the colour of the node depending on the state
        Renderer renderer = GetComponent<Renderer>();

        if (renderer)
        {
            switch (state)
            {
                case State.WAITING:
                    renderer.material.color = Color.gray;
                    break;
                case State.RUNNING:
                    renderer.material.color = Color.yellow;
                    break;
                case State.SUCCESS:
                    renderer.material.color = Color.green;
                    break;
                case State.FAILURE:
                    renderer.material.color = Color.red;
                    break;
            }
        }
    }

    public TreeNode.State RunNode()
    {
        // If the node is running, run its override function
        if (state == State.RUNNING)
        {
            NodeFunction();
        }

        return state;
    }

    protected virtual void NodeFunction()
    {
        // Do nothing, base class which needs different sections for selectors and leaf nodes
    }

    public virtual void ResetNode()
    {
        state = State.WAITING;
    }

    public void StartNode()
    {
        // Only allow node to run if it is waiting to run. 
        // Otherwise we need to reset the node
        if (state == State.WAITING)
        {
            state = State.RUNNING;
        }
    }

    public TreeNode.State GetNodeState()
    {
        return state;
    }

    public virtual void ForceStateChange(State newState)
    {
        // Allow for other nodes for force a state change. Make this virtual so it can be overwritten with instance specific code
        state = newState;
    }
}
