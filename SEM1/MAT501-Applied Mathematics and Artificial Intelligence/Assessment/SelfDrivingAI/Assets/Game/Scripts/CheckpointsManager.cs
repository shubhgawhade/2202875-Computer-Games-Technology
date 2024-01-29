using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CheckpointsManager : MonoBehaviour
{
    [SerializeField] 
    public GameObject[] cp;
    
    public static GameObject[] checkpoints;

    private void Awake()
    {
        checkpoints = cp;
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
