# -*- coding: utf-8 -*-
"""
MAT501 Practical 2
The Singular Value Decomposition and Principal Component Analysis

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

# RUN LLE ALGORITHM
embedding = LocallyLinearEmbedding(n_neighbors=12,n_components=2)
Y = embedding.fit_transform(X)

# SCATTERPLOT OF EMBEDDING
#fig = plt.figure()
#plt.title("A")
#plt.scatter(Y[:,0],Y[:,1],s=12,c=tt,marker='+')
#plt.show()

# RUN SPECTRAL EMBEDDING ALGORITHM
spectral = SpectralEmbedding(n_components=2)
Y_SPEC = spectral.fit_transform(X)

# SCATTERPLOT OF SPECTRAL EMBEDDING
#fig = plt.figure()
#plt.scatter(Y_SPEC[:,0],Y_SPEC[:,1],s=12,c=tt,marker='+')
#plt.show()

# PCA via SCIKIT LEARN FUNCTION
pca = PCA()
Y_PCA = pca.fit_transform(X)

# SCATTERPLOT OF PCA with d=2
#fig = plt.figure()
#plt.scatter(Y_PCA[:,0],Y_PCA[:,1],s=12,c=tt,marker='+')
#plt.show()

U,S,V = np.linalg.svd(X)
m, n =  np.shape(X)
# Create full Sigma matrix from singular values in S
sigma = np.zeros((m, n))
for i in range(min(m, n)):
    sigma[i, i] = S[i]
X1 = np.dot(U, np.dot(sigma, V))
assert(np.allclose(X, X1)) #check that the SVD matrices are correct by recovering X

fig = plt.figure()
plt.scatter(X1[:,0],X1[:,1],s=12,c=tt,marker='+')
plt.show()