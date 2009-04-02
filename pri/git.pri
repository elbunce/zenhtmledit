!win32 {
    exists($$PWD/../.git/HEAD) {
        GITVERSION=$$system(git log -n1 --pretty=format:%h)
        DEFINES += GITVERSION=\"\\\"$$GITVERSION\\\"\"
        GITCHANGENUMBER=$$system(git log --pretty=format:%h | wc -l)
        DEFINES += GITCHANGENUMBER=\"\\\"$$GITCHANGENUMBER\\\"\"
    } else {
        DEFINES += GITVERSION=\"\\\"0\\\"\"
        DEFINES += GITCHANGENUMBER=\"\\\"0\\\"\"
    }
} else {
    DEFINES += GITVERSION=0
    DEFINES += GITCHANGENUMBER=0
}
