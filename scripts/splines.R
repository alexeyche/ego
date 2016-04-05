#!/usr/bin/env Rscript

get.env = function(p, def=NULL) {
    v = Sys.getenv(p)
    if (v == "") {
        return (def)
    } else {
        return (v)
    }
}

args = commandArgs(trailingOnly = FALSE)

seed = get.env("SEED", -1)
if (seed > 0) {
    set.seed(seed)
} else {
    custom_seed = as.numeric(Sys.time())
    set.seed(custom_seed)
}

sleep = get.env("SLEEP", 0)
Sys.sleep(sleep)

x = NULL
in.r.studio = TRUE
if(length(grep("RStudio", args)) == 0) {
    f <- file("stdin")
    open(f)
    line = readLines(f,n=1)
    x = as.numeric(line)
    in.r.studio = FALSE
} else {
    #set.seed(100)
}

p.num = 4*10
stdevs = c(1, 2.5, 5.0, 7.5)



x.spl = seq(0, 1, length.out=p.num)
y.spl = NULL

for (std in stdevs) {
    y.spl = c(y.spl, rnorm(p.num/length(stdevs), 0, std))
}
y.spl = sample(y.spl)
spl = splinefun(x.spl, y.spl)
if (in.r.studio) {
    plot(spl, xlim = c(min(x.spl), max(x.spl)))
}
cat(spl(x), "\n")





