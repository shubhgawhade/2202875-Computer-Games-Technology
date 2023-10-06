# -*- coding: utf-8 -*-
"""
MAT501 Practical 4
AI Performance Analysis and Cross-Validation

Kean Lee Kang
Abertay University
2020
"""
import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import fetch_openml
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import make_pipeline
from sklearn.linear_model import LogisticRegression, SGDClassifier, Perceptron
from sklearn.linear_model import PassiveAggressiveClassifier
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.naive_bayes import BernoulliNB
from sklearn.model_selection import train_test_split
from sklearn import metrics
from sklearn.model_selection import StratifiedKFold


# Fetch open data for analysis (binary blood donation data)
# https://www.openml.org/d/1464
X, y = fetch_openml(data_id=1464, return_X_y=True)

# Alternatively obtain the same data from a CSV (download from MyLearningSpace)
#dataset = np.genfromtxt('OpenML_1464.csv', delimiter=',', skip_header=True)
#X = dataset[:,:-1] #everything except the last column in the dataset is X
#y = dataset[:,-1] #the last column in the dataset is y

# Make a pipeline to train a simple logistic regression model and fit to data

# Note: Try different classifiers and settings below as assigned and report results
# Logistic Regression
clf = make_pipeline(StandardScaler(), LogisticRegression(random_state=0))

# SGD
#clf = make_pipeline(StandardScaler(), SGDClassifier(max_iter=100))

# Perceptron
#clf = make_pipeline(StandardScaler(), Perceptron())

# Multi Layer Perceptron (Deep Neural Network)
#clf = make_pipeline(StandardScaler(), MLPClassifier(hidden_layer_sizes=(32,16,8), max_iter=300))

# Random Forest
#clf = make_pipeline(StandardScaler(), RandomForestClassifier(n_estimators=100))

# Gradient Boosting Classifier
#clf = make_pipeline(StandardScaler(), GradientBoostingClassifier(n_estimators=100))

# Naive Bayes (Bernoulli)
#clf = make_pipeline(StandardScaler(),BernoulliNB())

# Perform a single random train-test split and fit to the classifier
X_train, X_test, y_train, y_test = train_test_split(X, y, stratify=y)
clf.fit(X_train, y_train)

# Output results of classification using a single train-test split
metrics.plot_confusion_matrix(clf, X_test, y_test)
metrics.plot_roc_curve(clf, X_test, y_test)
metrics.plot_precision_recall_curve(clf, X_test, y_test)
plt.show()

# Using stratified K-fold cross-validation instead
cv = StratifiedKFold(n_splits=5)
tprs = []
aucs = []
mean_fpr = np.linspace(0, 1, 100)
precisions = []
aps = []
mean_recall = np.linspace(0, 1, 100)

fig, ax = plt.subplots()
fig2, ax2 = plt.subplots()
for i, (train, test) in enumerate(cv.split(X, y)):
    clf.fit(X.iloc[train,:], y.iloc[train])
    viz = metrics.plot_roc_curve(clf, X.iloc[test,:], y.iloc[test],
                         name='ROC fold {}'.format(i+1),
                         alpha=0.3, lw=1, ax=ax)
    viz2 = metrics.plot_precision_recall_curve(clf, X.iloc[test,:], y.iloc[test],
                         name='PR fold {}'.format(i+1),
                         alpha=0.3, lw=1, ax=ax2)    
    interp_tpr = np.interp(mean_fpr, viz.fpr, viz.tpr)
    list1, list2 = (list(t) for t in zip(*sorted(zip(list(viz2.recall), 
                                                     list(viz2.precision)))))
    recall_sorted = np.array(list1) #sorting is needed before interpolation
    precision_sorted = np.array(list2)
    interp_precision = np.interp(mean_recall, recall_sorted, precision_sorted)
    interp_tpr[0] = 0.0
    interp_precision[0] = 1.0
    tprs.append(interp_tpr)
    precisions.append(interp_precision)
    aucs.append(viz.roc_auc)
    aps.append(viz2.average_precision)

ax.plot([0, 1], [0, 1], linestyle='--', lw=2, color='r',
        label='Chance', alpha=.8)

mean_tpr = np.mean(tprs, axis=0)
mean_precision = np.mean(precisions, axis=0)
mean_tpr[-1] = 1.0
mean_precision[0] = 1.0
mean_auc = metrics.auc(mean_fpr, mean_tpr)
mean_ap = sum(aps)/len(aps)
std_auc = np.std(aucs)
std_ap = np.std(aps)
ax.plot(mean_fpr, mean_tpr, color='b',
        label=r'Mean ROC (AUC = %0.2f $\pm$ %0.2f)' % (mean_auc, std_auc),
        lw=2, alpha=.8)
ax2.plot(mean_recall, mean_precision, color='b',
        label=r'Mean PR (AP = %0.2f $\pm$ %0.2f)' % (mean_ap, std_ap),
        lw=2, alpha=.8)

std_tpr = np.std(tprs, axis=0)
std_precision = np.std(precisions, axis=0)
tprs_upper = np.minimum(mean_tpr + std_tpr, 1)
precisions_upper = np.minimum(mean_precision + std_precision, 1)
tprs_lower = np.maximum(mean_tpr - std_tpr, 0)
precisions_lower = np.maximum(mean_precision - std_precision, 0)
ax.fill_between(mean_fpr, tprs_lower, tprs_upper, color='grey', alpha=.2,
                label=r'$\pm$ 1 std. dev.')
ax2.fill_between(mean_recall, precisions_lower, precisions_upper, color='grey', alpha=.2,
                label=r'$\pm$ 1 std. dev.')

ax.set(xlim=[-0.05, 1.05], ylim=[-0.05, 1.05],
       title="Receiver operating characteristic cross-validated")
ax2.set(xlim=[-0.05, 1.05], ylim=[-0.05, 1.05],
       title="Precision recall curve cross-validated")
ax.legend(loc="lower right")
ax2.legend(loc="lower right")
plt.show()
