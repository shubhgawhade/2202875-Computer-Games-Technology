using System.Collections.Generic;
using System.Linq;
using Unity.VisualScripting;
using UnityEngine;

public class Coin
{
    public int id;
    public GameObject coinObject;
}

public class CoinSpawner : MonoBehaviour
{
    [SerializeField] private GameObject coinPrefab;
    public GameObject[] coinGameObjects;
    public List<Coin> coinsArray = new List<Coin>();
    
    public static List<Coin> CoinsList = new List<Coin>();
    
    private void Awake()
    {
        if (coinGameObjects.Length > 0)
        {
            for (int i = 0; i < coinGameObjects.Length; i++)
            {
                coinsArray.Add(new Coin());
                coinsArray[i].id = i;
                coinsArray[i].coinObject = coinGameObjects[i];
            }
            
            CoinsList = coinsArray.ToList();
        }
        else
        {
            // SpawnCoins();
        }
        
        // GameObject tempCoin = Instantiate(coinPrefab, new Vector3(5, 0, 2), Quaternion.identity);
        // tempCoin.AddComponent<Coin>().id = 0;
        // coinsArray[0] = tempCoin;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    // void SpawnCoins()
    // {
    //     int numberOfCoins = 15;
    //     CoinsArray = new Coin[15];
    //
    //     for (int i = 0; i < 15; i++)
    //     {
    //         Vector3 tempCoinPos = Vector3.zero;
    //
    //         int collisions;
    //         do
    //         {
    //             tempCoinPos = new Vector3(Random.Range(-10, 10), 0, Random.Range(-10, 10));
    //             Collider[] col = Physics.OverlapBox(tempCoinPos, coinPrefab.transform.localScale / 2,
    //                 Quaternion.identity);
    //             collisions = col.Length;
    //
    //         } while (collisions > 1);
    //
    //         GameObject tempCoin = Instantiate(coinPrefab, tempCoinPos, Quaternion.identity);
    //         
    //         CoinsArray[i].id = i;
    //         CoinsArray[i].coinObject = tempCoin;
    //     }
    // }
}