using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SelectorNode : TreeNode
{
    protected List<TreeNode> childNodes;

    // Start is called before the first frame update
    protected void Start()
    {
        // Add the TreeNodes from all child nodes to the list
        childNodes = new List<TreeNode>();
        for(int i = 0; i < this.transform.childCount; i++)
        {
            TreeNode node = this.transform.GetChild(i).gameObject.GetComponent<TreeNode>();
            if (node != null)
            {
                childNodes.Add(node);

                // Create the links between each node in the tree by instantiating a cylinder prefab and rotating it
                GameObject prefab = (GameObject)Resources.Load("Cylinder", typeof(GameObject));
                if (prefab != null)
                {
                    GameObject variableForPrefab = Instantiate(prefab);
                    float distance = Vector3.Distance(node.transform.position, this.transform.position);
                    variableForPrefab.transform.localScale = new Vector3(0.2f, distance / 2f, 0.2f);
                    variableForPrefab.transform.position = this.transform.position + ((node.transform.position - this.transform.position) / 2f);
                    variableForPrefab.transform.LookAt(this.transform, new Vector3(1, 0, 0));
                    variableForPrefab.transform.Rotate(new Vector3(1, 0, 0), 90);
                }
            }
        }

        
    }

    public override void ResetNode()
    {
        // If a seletor node is asked to reset, reset all child nodes
        base.ResetNode();
        foreach(TreeNode child in childNodes)
        {
            child.ResetNode();
        }
    }
}
