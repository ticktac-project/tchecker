
```
{

    "name" : string,

    "processes" : [
        { 
            "pid" : string,       
            "locations" : [
                {
                    "id" : string
                    "attributes" : {
                        "attr" : "value",
                        ...
                    }
                },
                ...            
            ],
            "edges" : [
                {
                    "src" : string,
                    "tgt" : string,
                    "attributes" : {
                        "attr" : "value",
                        ...
                    }
                }
            ]
       },
       ...    
    },
    
    "sync" : [
        [ "label1", ..., "labeln"],
        ...
    ]
}
```
