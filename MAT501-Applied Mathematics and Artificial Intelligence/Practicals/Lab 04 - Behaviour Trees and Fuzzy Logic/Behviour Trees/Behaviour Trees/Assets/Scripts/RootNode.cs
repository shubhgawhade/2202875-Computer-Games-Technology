using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RootNode : MonoBehaviour
{
    TreeNode node;
    bool running = false;

    // Grab a link to the TreeNode class
    void Start()
    {
        node = this.GetComponent<TreeNode>();
    }

    // Update is called once per frame
    void Update()
    {
        if(node != null)
        {
            if (running)
            {
                // Run the tree node
                TreeNode.State rootNodeState = node.RunNode();

                // Print status message
                if (rootNodeState == TreeNode.State.SUCCESS)
                {
                    Debug.Log("Tree has completed successfully. Can restart.");
                    running = false;
                }
                else if(rootNodeState == TreeNode.State.FAILURE)
                {
                    Debug.Log("Tree has failed. Can restart.");
                    running = false;
                }
            }
            else
            {
                // If we are not currently running the tree and the SPACEBAR is pressed, run the tree
                if (Input.GetKeyDown(KeyCode.Space))
                {
                    node.ResetNode();
                    node.StartNode();
                    running = true;
                }
            }
        }
    }
}
