import os, random, subprocess, itertools
D=os.path.join(os.path.dirname(os.path.abspath(__file__)),"")   # binaries gt2, gt2t built in this folder
rnd=random.Random(11); bad=0; N=600
for t in range(N):
    m=rnd.randint(2,9)
    allsets=[(i,) for i in range(m)]+list(itertools.combinations(range(m),2))+[()]
    cnt=rnd.randint(1,min(len(allsets),14))
    sets=rnd.sample(allsets,cnt)
    inp=f"{m} {len(sets)} "+" ".join(str(sum(1<<e for e in C)) for C in sets)+"\n"
    a=subprocess.run([D+"gt2t","tchi"],input=inp,capture_output=True,text=True).stdout.strip()
    b=subprocess.run([D+"gt2","chi"],input=inp,capture_output=True,text=True).stdout.strip()
    if a!=b:
        bad+=1; print("MISMATCH",m,sets,a,b)
print("random families:",N,"types-vs-colouring mismatches:",bad)
