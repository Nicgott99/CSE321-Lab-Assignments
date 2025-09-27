#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 5
#define MAX_RESOURCES 5
#define MAX_NAME_LEN 20

typedef enum {
    READ = 1,    
    WRITE = 2,       
    EXECUTE = 4   
} Permission;

// User and Resource Definitions
typedef struct {
    char name[MAX_NAME_LEN];
} User;

typedef struct {
    char name[MAX_NAME_LEN];
} Resource;

// ACL Entry structure
typedef struct {
    char username[MAX_NAME_LEN];
    int permissions;  
} ACLEntry;

typedef struct {
    Resource resource;
    ACLEntry acl_entries[MAX_USERS];
    int acl_count;
} ACLControlledResource;

// Capability Entry structure  
typedef struct {
    char resource_name[MAX_NAME_LEN];
    int permissions;  // Stores combined permissions as bitmask
} Capability;

typedef struct {
    User user;
    Capability capabilities[MAX_RESOURCES];
    int capability_count;
} CapabilityUser;

// Utility Functions
void printPermissions(int perm) {
    if (perm == 0) {
        printf("NONE");
        return;
    }
    
    int count = 0;
    if (perm & READ) {
        if (count > 0) printf(", ");
        printf("READ");
        count++;
    }
    if (perm & WRITE) {
        if (count > 0) printf(", ");
        printf("WRITE");
        count++;
    }
    if (perm & EXECUTE) {
        if (count > 0) printf(", ");
        printf("EXECUTE");
        count++;
    }
}

int hasPermission(int userPerm, int requiredPerm) {
    return (userPerm & requiredPerm) == requiredPerm;
}

// ACL System Implementation
void checkACLAccess(ACLControlledResource *res, const char *userName, int perm) {
    printf("ACL Check: User %s requests ", userName);
    printPermissions(perm);
    printf(" on %s: ", res->resource.name);
    
    for (int i = 0; i < res->acl_count; i++) {
        if (strcmp(res->acl_entries[i].username, userName) == 0) {
            if (hasPermission(res->acl_entries[i].permissions, perm)) {
                printf("Access GRANTED\n");
            } else {
                printf("Access DENIED\n");
            }
            return;
        }
    }
    
    printf("Access DENIED\n");
}

// Capability System Implementation
void checkCapabilityAccess(CapabilityUser *user, const char *resourceName, int perm) {
    printf("Capability Check: User %s requests ", user->user.name);
    printPermissions(perm);
    printf(" on %s: ", resourceName);
    
    for (int i = 0; i < user->capability_count; i++) {
        if (strcmp(user->capabilities[i].resource_name, resourceName) == 0) {
            if (hasPermission(user->capabilities[i].permissions, perm)) {
                printf("Access GRANTED\n");
            } else {
                printf("Access DENIED\n");
            }
            return;
        }
    }
    
    printf("Access DENIED\n");
}

// Helper function to add ACL entry
void addACLEntry(ACLControlledResource *res, const char *username, int permissions) {
    if (res->acl_count < MAX_USERS) {
        strcpy(res->acl_entries[res->acl_count].username, username);
        res->acl_entries[res->acl_count].permissions = permissions;
        res->acl_count++;
    }
}

// Helper function to add capability
void addCapability(CapabilityUser *user, const char *resourceName, int permissions) {
    if (user->capability_count < MAX_RESOURCES) {
        strcpy(user->capabilities[user->capability_count].resource_name, resourceName);
        user->capabilities[user->capability_count].permissions = permissions;
        user->capability_count++;
    }
}

int main() {
    User users[MAX_USERS] = {{"Alice"}, {"Bob"}, {"Charlie"}, {"Diana"}, {"Edward"}};
    Resource resources[MAX_RESOURCES] = {{"File1"}, {"File2"}, {"File3"}, {"Database1"}, {"Script1"}};
    
    ACLControlledResource aclResources[MAX_RESOURCES];
    
    for (int i = 0; i < MAX_RESOURCES; i++) {
        strcpy(aclResources[i].resource.name, resources[i].name);
        aclResources[i].acl_count = 0;
    }
    
    // Configure ACL permissions for File1
    addACLEntry(&aclResources[0], "Alice", READ | WRITE);
    addACLEntry(&aclResources[0], "Bob", READ);
    addACLEntry(&aclResources[0], "Charlie", EXECUTE);
    
    // Configure ACL permissions for File2
    addACLEntry(&aclResources[1], "Alice", READ | EXECUTE);
    addACLEntry(&aclResources[1], "Diana", WRITE | READ);
    
    // Configure ACL permissions for File3
    addACLEntry(&aclResources[2], "Bob", READ | WRITE | EXECUTE);
    addACLEntry(&aclResources[2], "Edward", READ);
    
    // Configure ACL permissions for Database1
    addACLEntry(&aclResources[3], "Diana", READ | WRITE);
    addACLEntry(&aclResources[3], "Alice", READ);
    
    // Configure ACL permissions for Script1
    addACLEntry(&aclResources[4], "Edward", EXECUTE | READ);
    addACLEntry(&aclResources[4], "Charlie", EXECUTE);
    
    // Capability Setup - Initialize capability users
    CapabilityUser capabilityUsers[MAX_USERS];
    
    // Setup each user with empty capability list initially
    for (int i = 0; i < MAX_USERS; i++) {
        strcpy(capabilityUsers[i].user.name, users[i].name);
        capabilityUsers[i].capability_count = 0;
    }
    
    // Configure Alice's capabilities
    addCapability(&capabilityUsers[0], "File1", READ | WRITE);
    addCapability(&capabilityUsers[0], "File2", READ | EXECUTE);
    addCapability(&capabilityUsers[0], "Database1", READ);
    
    // Configure Bob's capabilities
    addCapability(&capabilityUsers[1], "File1", READ);
    addCapability(&capabilityUsers[1], "File3", READ | WRITE | EXECUTE);
    
    // Configure Charlie's capabilities  
    addCapability(&capabilityUsers[2], "File1", EXECUTE);
    addCapability(&capabilityUsers[2], "Script1", EXECUTE);
    
    // Configure Diana's capabilities
    addCapability(&capabilityUsers[3], "File2", WRITE | READ);
    addCapability(&capabilityUsers[3], "Database1", READ | WRITE);
    
    // Configure Edward's capabilities
    addCapability(&capabilityUsers[4], "File3", READ);
    addCapability(&capabilityUsers[4], "Script1", EXECUTE | READ);
    
    // Test ACL System
    printf("========== ACL Access Control Tests ==========\n");
    
    // Original test cases
    checkACLAccess(&aclResources[0], "Alice", READ);
    checkACLAccess(&aclResources[0], "Bob", WRITE);
    checkACLAccess(&aclResources[0], "Charlie", READ);
    
    // Additional test cases for new resources and users
    checkACLAccess(&aclResources[1], "Alice", EXECUTE);
    checkACLAccess(&aclResources[1], "Diana", WRITE);
    checkACLAccess(&aclResources[2], "Bob", EXECUTE);
    checkACLAccess(&aclResources[2], "Edward", WRITE);
    checkACLAccess(&aclResources[3], "Diana", READ);
    checkACLAccess(&aclResources[3], "Alice", WRITE);
    checkACLAccess(&aclResources[4], "Edward", EXECUTE);
    checkACLAccess(&aclResources[4], "Charlie", READ);
    
    printf("\n");
    
    // Test Capability System
    printf("========== Capability-Based Access Control Tests ==========\n");
    
    // Original test cases
    checkCapabilityAccess(&capabilityUsers[0], "File1", WRITE);
    checkCapabilityAccess(&capabilityUsers[1], "File1", WRITE);
    checkCapabilityAccess(&capabilityUsers[2], "File2", READ);
    
    // Additional test cases for new resources and users
    checkCapabilityAccess(&capabilityUsers[0], "File2", EXECUTE);
    checkCapabilityAccess(&capabilityUsers[1], "File3", READ);
    checkCapabilityAccess(&capabilityUsers[2], "Script1", EXECUTE);
    checkCapabilityAccess(&capabilityUsers[3], "Database1", WRITE);
    checkCapabilityAccess(&capabilityUsers[4], "Script1", READ);
    checkCapabilityAccess(&capabilityUsers[3], "File3", READ);
    
    return 0;
}
