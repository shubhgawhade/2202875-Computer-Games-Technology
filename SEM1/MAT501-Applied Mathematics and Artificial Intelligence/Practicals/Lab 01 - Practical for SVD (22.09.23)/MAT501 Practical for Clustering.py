# -*- coding: utf-8 -*-
"""
MAT501 Practical 2
Clustering

Kean Lee Kang
Abertay University
2021
"""
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
import math
from sklearn.manifold import LocallyLinearEmbedding
from sklearn.manifold import SpectralEmbedding
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans

N = 2000
K = 12
d = 2
n_clusters = 3
random_state = 41

# PLOT TRUE MANIFOLD
tt0 = (3*math.pi/2)*(1+2*np.linspace(0,1,num=501))
hh = np.linspace(0,1,num=99)*30
xx = np.outer(np.multiply(tt0,np.cos(tt0)),np.ones(np.shape(hh)));
yy = np.outer(np.ones(np.shape(tt0)),hh)
zz = np.outer(np.multiply(tt0,np.sin(tt0)),np.ones(np.shape(hh)));
cc = np.outer(tt0,np.ones(np.shape(hh)))

# Plot the surface.
#fig = plt.figure()
#ax = fig.add_subplot(111, projection='3d')
#surf = ax.plot_surface(xx, yy, zz, cmap=cm.viridis, #perceptually uniform cmap
#                       linewidth=0, antialiased=False)
# Add a color bar which maps values to colors.
#fig.colorbar(surf, shrink=0.5, aspect=5)
#plt.show()

# GENERATE SAMPLED DATA
tt = (3*math.pi/2)*(1+2*np.random.random_sample(N))  
height = 21*np.random.random_sample(N)
X = np.column_stack((np.multiply(tt,np.cos(tt)),height,np.multiply(tt,np.sin(tt))))

# SCATTERPLOT OF SAMPLED DATA
#fig = plt.figure()
#ax = fig.add_subplot(111, projection='3d')
#ax.scatter(X[:,0],X[:,1],X[:,2],s=12,c=tt,marker='+')
#ax.view_init(elev=10, azim=290)
#for ii in range(0,360,1):
#    ax.view_init(elev=10., azim=ii)
#    plt.savefig('movie%d.png' % ii)
#plt.show()

embedding = LocallyLinearEmbedding(n_neighbors=12,n_components=2)
Y = embedding.fit_transform(X)

# CLUSTERING OF SAMPLED DATA
kmeans = KMeans(n_clusters=n_clusters, random_state=random_state)
y_pred = kmeans.fit_predict(Y)
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(Y[:,0],Y[:,1],s=12,c=y_pred,marker='+')
#ax.scatter(X[:,0],X[:,1],X[:,2],s=12,c=y_pred,marker='+')
centroids = kmeans.cluster_centers_
ax.scatter(centroids[:,0],centroids[:,1], marker="x", s=169, color="k")
ax.view_init(elev=90, azim=290)
plt.title("KMeans with n_clusters="+str(len(centroids)))
plt.show()
