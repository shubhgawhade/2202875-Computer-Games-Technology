using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// This node runs in opposite to the SequenceSelector.

public class FallbackSelector : SelectorNode
{
    protected override void NodeFunction()
    {
        // Flag to see if at least one child node has run
        bool hasRunChildNode = false;

        // Iterate over each child node
        foreach (TreeNode node in childNodes)
        {
            // Start Node - does nothing if already running
            node.StartNode();

            // If the child node has already failed, move to the next one
            if (node.GetNodeState() == State.FAILURE)
                continue;

            // State we have run a child node - we are about to
            hasRunChildNode = true;

            // Run the node and check if it succeeds. If it does set this node to have succeeded
            if (node.RunNode() == State.SUCCESS)
            {
                this.state = State.SUCCESS;
            }

            // Once a single child node has been run, break from the for loop
            break;
        }

        // If not child nodes have been run, this selector has failed as well
        if (!hasRunChildNode)
        {
            this.state = State.FAILURE;
        }
    }
}
