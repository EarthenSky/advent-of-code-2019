using System;
using System.Collections.Generic;
using System.IO;

public class Program 
{
    public static void Main(string[] args) {
        Analysis a = new Analysis();
        a.FillFromFile("input");
        
        Console.WriteLine("total orbits = {0}", a.GetOrbitsCount());
        Console.WriteLine("distance between SAN and YOU = {0}", a.FindDistance("SAN", "YOU"));
    } 
}

// This is a tree node.
public class ObjectNode 
{   
    private ObjectNode parent;
    private List<ObjectNode> children;

    public string Name;
    public int Depth;
    
    public ObjectNode(string name) {
        this.parent = null;
        children = new List<ObjectNode>();

        Name = name;
        Depth = -1;  // -1 means undefined;
    }

    public void AddChild(ObjectNode child) {
        children.Add(child);
    }

    public List<ObjectNode> GetChildren() {
        return children;
    }

    public ObjectNode GetParent() { 
        return parent; 
    }

    public void SetParent(ObjectNode parent) {
        this.parent = parent;
    }
}

// this is the code for part 1
public class Analysis
{
    private ObjectNode treeRoot;
    private List<ObjectNode> objects;
    private bool allItemsHaveDepth;
    
    public Analysis() {
        objects = new List<ObjectNode>();
        allItemsHaveDepth = false;
    }

    // Fills the object list with Tree nodes that are connected.
    public void FillFromFile(string filename) {
        var lines = File.ReadLines(filename);
        foreach (var line in lines) {
            string parent = line.Substring(0, 3);
            string child = line.Substring(4, 3);

            ObjectNode parentNode = GetObject(parent, true);
            ObjectNode childNode = GetObject(child, true);
            
            parentNode.AddChild(childNode);
            childNode.SetParent(parentNode);
        }

        InitRoot();  // find the root by traversing upwards through the parents.
    }

    private void InitRoot() {
        ObjectNode tmp = objects[1]; 
        while(tmp.GetParent() != null)
            tmp = tmp.GetParent();
        
        treeRoot = tmp;
        treeRoot.Depth = 0;
    }

    private ObjectNode GetObject(string name, bool createIfNotFound) {
        foreach(ObjectNode obj in objects) {
            if(obj.Name == name) return obj;
        }
        
        if(createIfNotFound) return CreateObject(name);
        else return null;
    }

    private ObjectNode CreateObject(string name) {
        ObjectNode tmp = new ObjectNode(name);
        objects.Add(tmp);
        return tmp; 
    }

    // After this call, all orbits should have a depth.
    public int GetOrbitsCount() {
        int counter = 0;
        List<ObjectNode> queue = new List<ObjectNode>();
        queue.Add(treeRoot);  // initial node
        
        while(queue.Count != 0) {
            // Add Child nodes to the queue 
            ObjectNode tmp = queue[0];
            queue.RemoveAt(0);
            foreach(ObjectNode node in tmp.GetChildren()) {
                queue.Add(node);
            }

            // Get depth from parent ~ pseudo recursively.
            if(tmp.Depth == -1) tmp.Depth = tmp.GetParent().Depth + 1;
            counter += tmp.Depth;
        }

        return counter;
    }

    // This function assumes that San and You are not orbiting each other.
    public int FindDistance(string a, string b) {
        if(!allItemsHaveDepth) GetOrbitsCount();

        int counter = 0;
        ObjectNode aNode = GetObject(a, false).GetParent();
        ObjectNode bNode = GetObject(b, false).GetParent();

        // step upwards on one side until both objects are at the same depth.
        while(aNode.Depth < bNode.Depth) {
            counter++;
            bNode = bNode.GetParent(); 
        }

        while(aNode.Depth > bNode.Depth) {
            counter++;
            aNode = aNode.GetParent();
        }

        // Continue Comparing parents until they share the same parent.
        while(aNode.Name != bNode.Name) {
            counter += 2;
            aNode = aNode.GetParent();
            bNode = bNode.GetParent(); 
        }

        return counter;
    }
} 

