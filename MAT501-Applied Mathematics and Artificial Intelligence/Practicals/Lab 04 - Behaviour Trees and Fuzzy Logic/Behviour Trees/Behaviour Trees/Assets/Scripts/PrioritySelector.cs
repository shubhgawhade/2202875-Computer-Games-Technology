using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PrioritySelector : SelectorNode
{
    [System.Serializable]
    class PriorityClass
    {
        public TreeNode node;
        public int priorityValue;
    }

    [SerializeField] private List<PriorityClass> childPriorities = new();

    // Start is called before the first frame update
    void Start()
    {
        base.Start();

        for (int i = 0; i < childNodes.Count; i++)
        {
            PriorityClass temp = new PriorityClass
            {
                node = childNodes[i]
            };
            
            childPriorities.Add(temp);

            switch (i)
            {
                case 0:
                    
                    childPriorities[i].priorityValue = 2;

                    break;
                
                case 1:
                    
                    childPriorities[i].priorityValue = 1;

                    break;
                
                case 2:
                    
                    childPriorities[i].priorityValue = 3;

                    break;
            }
        }
    }

    protected override void NodeFunction()
    {
        // Flag to see if at least one child node has run
        bool hasRunChildNode = false;

        childPriorities.Sort(delegate(PriorityClass c1, PriorityClass c2) { return c1.priorityValue.CompareTo(c2.priorityValue); });

        // Iterate over each child node
        foreach(PriorityClass child in childPriorities)
        {
            // Start Node - does nothing if already running
            child.node.StartNode();
        
            // If the child node has already finished then move to next child node in the list
            if (child.node.GetNodeState() == State.SUCCESS)
                continue;
        
            // State we have run a child node - we are about to
            hasRunChildNode = true;
        
            // Run the node and check if it fails. If it does set this node to have failed
            if (child.node.RunNode() == State.FAILURE)
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
