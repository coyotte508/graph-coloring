import matplotlib.pyplot as plt


X = [0.4, 0.5, 0.6, 0.7, 0.8]
greedy = [65.68, 78.407, 76.939, 52.087, 28.393]
dsat = [63.974, 65.017, 68.996, 46.943, 25.304]
#rlf = [69.355, 81.483, 68.293, 43.778, 23.07]
rlfs = [64.489, 71.876, 72.296, 49.569, 27.457]
rlfbest = [64.489, 71.876, 68.293, 43.778, 23.07]
neural = [63.697, 62.502, 53.848, 36.213, 20.019]

dsat2 = [63.967, 63.989, 74.023, 65.566, 35.224]
rlf2 = [63.999, 66.406, 83.002, 68.278, 37.934]
greedy2 = [64.362, 70.606, 94.185, 72.698, 39.472]
neural2 = [63.92, 63.924, 64.748, 48.99, 27.256]

plt.plot(X, greedy, marker="x", label="greedy")
plt.plot(X, dsat, marker="v", label="DSATUR")
plt.plot(X, rlfs, marker="^", label="RLF")
plt.plot(X, neural, marker="*", label="correl")


plt.plot(X, greedy2, "--", color='blue')
plt.plot(X, dsat2, "--", color='green')
plt.plot(X, rlf2, "--", color='red')
plt.plot(X, neural2, "--", color='cyan')

plt.legend(loc="upper right")

plt.ylabel("Number of colors (optimal: 64)")
plt.xlabel("Erasure probability p")

plt.gca().grid(True)

plt.show()
