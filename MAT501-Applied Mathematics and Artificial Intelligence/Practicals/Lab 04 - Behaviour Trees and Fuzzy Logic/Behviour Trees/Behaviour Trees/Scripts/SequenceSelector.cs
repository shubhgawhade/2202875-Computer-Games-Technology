using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// This node runs in opposite to the FallbackSelector.

public class SequenceSelector : SelectorNode
{
    protected override void NodeFunction()
    {
        // Flag to see if at least one child node has run
        bool hasRunChildNode = false;

        // Iterate over each child node
        foreach(TreeNode node in childNodes)
        {
            // Start Node - does nothing if already running
            node.StartNode();

            // If the child node has already finished then move to next child node in the list
            if (node.GetNodeState() == State.SUCCESS)
                continue;

            // State we have run a child node - we are about to
            hasRunChildNode = true;

            // Run the node and check if it fails. If it does set this node to have failed
            if (node.RunNode() == State.FAILURE)
            {
                this.state = State.FAILURE;
            }

            // Once a single child node has been run, break from the for loop
            break;
        }

        // If not child nodes have been run, this selector has succeeded
        if(!hasRunChildNode)
        {
            this.state = State.SUCCESS;
        }
    }
}
