import random,subprocess,time
random.seed(5)
for t in range(5):
    sigs=[random.randrange(32) for _ in range(14)]
    t0=time.time()
    out=subprocess.run(["./gt","eval","5","3"]+list(map(str,sigs)),capture_output=True,text=True).stdout
    print(sigs,out.split("\n")[-2],round(time.time()-t0,2))
