# -*- coding: utf-8 -*-
"""
CMP501 Practical Week 6
Markov Decision Processes

Kean Lee Kang
Abertay University
2021

Adapted from 
https://towardsdatascience.com/how-to-code-the-value-iteration-algorithm-for-reinforcement-learning-8fb806e117d1
https://github.com/francoisstamant/reinforcement-learning-mdp/blob/main/mdp.py
"""
print(__doc__)

import numpy as np
import matplotlib.pyplot as plt

'''==================================================
Initial set up
=================================================='''

#Hyperparameters
SMALL_ENOUGH = 0.05
GAMMA = 0.9         
NOISE = 0.2  

#Define all states
all_states=[]
for i in range(3):
    for j in range(4):
            all_states.append((i,j))

#Define rewards for all states
rewards = {}
for i in all_states:
    if i == (1,1):
        rewards[i] = -1
    elif i == (2,1):
        rewards[i] = -1
    elif i == (0,3):
        rewards[i] = 1
    else:
        rewards[i] = 0

#Dictionary of possible actions. We have two "end" states (1,2 and 2,2)
actions = {
    (0,0):('D', 'R'), 
    (0,1):('D', 'R', 'L'),    
    (0,2):('D', 'L', 'R'),
    #(0,3):('D', 'L'),
    (1,0):('D', 'U', 'R'),
    #(1,1):('D', 'R', 'L', 'U'),
    (1,2):('D', 'R', 'L', 'U'),
    (1,3):('D', 'L', 'U'),
    (2,0):('U', 'R'),
    #(2,1):('U', 'L', 'R'),
    (2,2):('U', 'L', 'R'),
    (2,3):('U', 'L'),
    }

#Define an initial policy
policy={}
for s in actions.keys():
    policy[s] = np.random.choice(actions[s])

#Define initial value function 
V={}
for s in all_states:
    if s in actions.keys():
        V[s] = 0
    if s ==(1,1):
        V[s]=-1
    if s == (2,1):
        V[s]=-1
    if s == (0,3):
        V[s]=1
        
'''==================================================
Value Iteration
=================================================='''

iteration = 0

while True:
    biggest_change = 0
    for s in all_states:            
        if s in policy:
            
            old_v = V[s]
            new_v = 0
            
            for a in actions[s]:
                if a == 'U':
                    nxt = [s[0]-1, s[1]]
                if a == 'D':
                    nxt = [s[0]+1, s[1]]
                if a == 'L':
                    nxt = [s[0], s[1]-1]
                if a == 'R':
                    nxt = [s[0], s[1]+1]

                #Choose a new random action to do (transition probability)
                random_1=np.random.choice([i for i in actions[s] if i != a])
                if random_1 == 'U':
                    act = [s[0]-1, s[1]]
                if random_1 == 'D':
                    act = [s[0]+1, s[1]]
                if random_1 == 'L':
                    act = [s[0], s[1]-1]
                if random_1 == 'R':
                    act = [s[0], s[1]+1]

                #Calculate the value
                nxt = tuple(nxt)
                act = tuple(act)
                v = rewards[s] + (GAMMA * ((1-NOISE)* V[nxt] + (NOISE * V[act]))) 
                if v > new_v: #Is this the best action so far? If so, keep it
                    new_v = v
                    policy[s] = a

       #Save the best of all actions for the state                                
            V[s] = new_v
            biggest_change = max(biggest_change, np.abs(old_v - V[s]))

            
   #See if the loop should stop now         
    if biggest_change < SMALL_ENOUGH:
        break
    iteration += 1
    
# Visualize results
# Plot rewards map
rewards_matrix = np.zeros((3,4))
for coordinates in rewards.keys():
    rewards_matrix[coordinates[0],coordinates[1]] = rewards[coordinates]
 
fig, ax = plt.subplots()
plt.title('Rewards Map')
ax.matshow(rewards_matrix, cmap='jet')
for (i, j), z in np.ndenumerate(rewards_matrix):
    ax.text(j, i, '{:0.1f}'.format(z), ha='center', va='center', color='white', fontsize=30)
plt.show()

# Plot values map
V_matrix = np.zeros((3,4))
for coordinates in V.keys():
    V_matrix[coordinates[0],coordinates[1]] = V[coordinates]
    
fig, ax = plt.subplots()
plt.title('Value Map')
ax.matshow(V_matrix, cmap='viridis')
for (i, j), z in np.ndenumerate(V_matrix):
    ax.text(j, i, '{:0.1f}'.format(z), ha='center', va='center', color='white', fontsize=30)
plt.show()
6
# Plot policy map
policy_matrix = np.zeros((3,4))
policy_dict = {'U':0.25, 'D':0.5, 'L':0.75, 'R':1}
policy_inverse_dict = {0:'', 0.25:'Up', 0.5:'Down', 0.75:'Left', 1:'Right'}
#policy_dict = {'U':0.5, 'D':0.25, 'L':0.25, 'R':1}
#policy_inverse_dict = {0:'', 0.5:'Up', 0.3:'Down', 0.25:'Left', 1:'Right'}
for coordinates in policy.keys():
    policy_matrix[coordinates[0],coordinates[1]] = policy_dict[policy[coordinates]]

fig, ax = plt.subplots()
plt.title('Policy Map')
ax.matshow(policy_matrix, cmap='jet')
for (i, j), z in np.ndenumerate(policy_matrix):
    ax.text(j, i, policy_inverse_dict[z], ha='center', va='center', color='white', fontsize=20)
plt.show()