#include <iostream>
#include <vector>
#include "item.hpp"

using namespace std;

int main() {
    ItemManager im;
    
    vector<Position> dummyBlocked = {{5, 5}, {6, 6}, {7, 7}};

    cout << "=== 1. Item Generation Test ===" << endl;
    im.generateItem(20, 20, dummyBlocked);
    for(int i = 0; i < im.items.size(); i++) {
        cout << "Item " << i+1 << ": (" << im.items[i].x << ", " << im.items[i].y 
             << ") Type: " << im.items[i].type << " Lifetime: " << im.items[i].lifetime << endl;
    }

    cout << "\n=== 2. Collision Test ===" << endl;
    if(!im.items.empty()) {
        int targetX = im.items[0].x;
        int targetY = im.items[0].y;
        cout << "Snake moves to: (" << targetX << ", " << targetY << ")" << endl;
        
        ItemType eaten = im.checkCollision(targetX, targetY);
        cout << "Eaten item type: " << eaten << endl;
        cout << "Remaining items in map: " << im.items.size() << endl;
    }

    cout << "\n=== 3. Lifetime Timer Test ===" << endl;
    for(int i = 0; i < 49; i++) {
        im.updateItems(20, 20, dummyBlocked);
    }
    cout << "After 49 turns -> First item lifetime: " << im.items[0].lifetime << ", Item count: " << im.items.size() << endl;
    
    im.updateItems(20, 20, dummyBlocked);
    cout << "After 50 turns (Expired & Regenerated) -> First item lifetime: " << im.items[0].lifetime << ", Item count: " << im.items.size() << endl;

    return 0;
}