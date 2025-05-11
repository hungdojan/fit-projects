-- Implementation of project assignment at FIT, BUT 2021/2022
-- This script implements database of vegan restaurant
--
-- Authors: Marek Dohnal (xdohna48@stud.fit.vutbr.cz)
--          Hung Do      (xdohun00@stud.fit.vutbr.cz)

-- TASK 1
DROP MATERIALIZED VIEW "OrderInsertionCounter";
DROP TABLE "Allergens" CASCADE CONSTRAINTS;
DROP TABLE "AmountOfItemsDish" CASCADE CONSTRAINTS;
DROP TABLE "AmountOfItemsDrink" CASCADE CONSTRAINTS;
DROP TABLE "User" CASCADE CONSTRAINTS;
DROP TABLE "Dish" CASCADE CONSTRAINTS;
DROP TABLE "DrinkIngredientsRelation" CASCADE CONSTRAINTS;
DROP TABLE "DishIngredientsRelation" CASCADE CONSTRAINTS;
DROP TABLE "Drink" CASCADE CONSTRAINTS;
DROP TABLE "AllergensDishRelation" CASCADE CONSTRAINTS;
DROP TABLE "AllergensDrinkRelation" CASCADE CONSTRAINTS;
DROP TABLE "Employee" CASCADE CONSTRAINTS;
DROP TABLE "Ingredient" CASCADE CONSTRAINTS;
DROP TABLE "Order" CASCADE CONSTRAINTS;
DROP TABLE "Patch" CASCADE CONSTRAINTS;
DROP TABLE "Payment" CASCADE CONSTRAINTS;
DROP TABLE "Reservation" CASCADE CONSTRAINTS;
DROP TABLE "ReservationRoomRelation" CASCADE CONSTRAINTS;
DROP TABLE "Room" CASCADE CONSTRAINTS;
DROP TABLE "RoomType" CASCADE CONSTRAINTS;

CREATE TABLE "User" (
    ID INT NOT NULL PRIMARY KEY,
    Email VARCHAR(256),
    PhoneNum VARCHAR(30),
    Name VARCHAR(256) NOT NULL,
    Surname VARCHAR(256),

                                                        -- RFC 5322 format
    CONSTRAINT EmailFormat CHECK ( REGEXP_LIKE(Email, '^[a-zA-Z0-9_!#$%&’*+/=?`{|}~^.-]+@[a-zA-Z0-9.-]+$') ),
                                                        -- basic Czech phone number (00420/+420) and 9 digits
    CONSTRAINT PhoneNumberFormat CHECK ( REGEXP_LIKE(PhoneNum, '^(\+420\s?|00420\s?)?[0-9]{3}\s?[0-9]{3}\s?[0-9]{3}$') )
);

CREATE TABLE "Employee" (
    ID INT NOT NULL PRIMARY KEY,
    DateOfBirth DATE,
    ContactType INT NOT NULL,
    Superior INT,
    ParentTable INT NOT NULL, -- pointer to parent table User

    CONSTRAINT FK_Superior FOREIGN KEY (Superior) REFERENCES "Employee",
    CONSTRAINT FK_ParentTable FOREIGN KEY (ParentTable) REFERENCES "User"
);

CREATE TABLE "Payment" (
    ID INT NOT NULL PRIMARY KEY,
    DateTime TIMESTAMP,
    Price FLOAT DEFAULT 0.0,
    PaymentType INT,
    CreatedBy INT NOT NULL,

    CONSTRAINT FK_CreatedBy FOREIGN KEY (CreatedBy) REFERENCES "Employee"
);

CREATE TABLE "Order" (
    ID INT NOT NULL PRIMARY KEY,
    OrderStatus INT,
    DateTime TIMESTAMP,
    ContainsPayment INT,
    CreatedBy INT,
    OrderedBy INT,

    CONSTRAINT OrdStatCondition CHECK ( OrderStatus >= 0 AND OrderStatus <= 1 ),
    CONSTRAINT FK_OrderCreatedBy FOREIGN KEY (CreatedBy) REFERENCES "Employee",
    CONSTRAINT FK_OrderOrderedBy FOREIGN KEY (OrderedBy) REFERENCES "User",
    CONSTRAINT FK_ContainsPayment FOREIGN KEY (ContainsPayment) REFERENCES "Payment"
);

CREATE TABLE "Dish" (
    ID INT NOT NULL PRIMARY KEY,
    Name VARCHAR(256) NOT NULL,
    PricePerItem FLOAT NOT NULL,
    AmountInStock INT,
    Weight INT,
    PortionSize INT,

    CONSTRAINT PortionSizeStatus CHECK ( PortionSize >= 0 AND PortionSize <= 1 )
);

CREATE TABLE "Drink" (
    ID INT NOT NULL PRIMARY KEY,
    Name VARCHAR(256) NOT NULL,
    PricePerItem FLOAT NOT NULL,
    AmountInStock INT,
    Volume INT
);


CREATE TABLE "Allergens" (
    Name VARCHAR(256) PRIMARY KEY
);

CREATE TABLE "AllergensDishRelation" (
    DishID INT NOT NULL,
    AllergenID VARCHAR(256) NOT NULL,

    CONSTRAINT FK_ADDish_DishID FOREIGN KEY (DishID) REFERENCES "Dish",
    CONSTRAINT FK_ADDish_AllergenID FOREIGN KEY (AllergenID) REFERENCES "Allergens",
    CONSTRAINT PK_ADDish PRIMARY KEY (DishID, AllergenID)
);

CREATE TABLE "AllergensDrinkRelation" (
    DrinkID INT NOT NULL,
    AllergenID VARCHAR(256) NOT NULL,

    CONSTRAINT FK_ADDrink_Drink FOREIGN KEY (DrinkID) REFERENCES "Drink",
    CONSTRAINT FK_ADDrink_AllergenID FOREIGN KEY (AllergenID) REFERENCES "Allergens",
    CONSTRAINT PK_ADDrink PRIMARY KEY (DrinkID, AllergenID)
);

CREATE TABLE "AmountOfItemsDrink" (
    OrderID INT NOT NULL,
    DrinkID INT NOT NULL,
    ItemCount INT NOT NULL,

    CONSTRAINT PK_AmountOfItemsDrink PRIMARY KEY (OrderID, DrinkID),
    CONSTRAINT FK_AmountOfItemsDrinkOrderID FOREIGN KEY (OrderID) REFERENCES "Order",
    CONSTRAINT FK_AmountOfItemsDrinkDrinkID FOREIGN KEY (DrinkID) REFERENCES "Drink"
);

CREATE TABLE "AmountOfItemsDish" (
     OrderID INT NOT NULL,
     DishID INT NOT NULL,
     ItemCount INT NOT NULL,

     CONSTRAINT PK_AmountOfItemsDish PRIMARY KEY (OrderID, DishID),
     CONSTRAINT FK_AmountOfItemsDishOrderID FOREIGN KEY (OrderID) REFERENCES "Order",
     CONSTRAINT FK_AmountOfItemsDishDishID FOREIGN KEY (DishID) REFERENCES "Dish"
);

CREATE TABLE "Ingredient" (
    ID INT NOT NULL PRIMARY KEY,
    Name VARCHAR(256),
    DateOfDelivery DATE,
    AmountInStock INT
);

CREATE TABLE "DishIngredientsRelation" (
    DishID INT NOT NULL,
    IngredientID INT NOT NULL,

    CONSTRAINT PK_DishIngredientsRelation PRIMARY KEY (IngredientID, DishID),
    CONSTRAINT FK_DishIngredientsRelation_DishID FOREIGN KEY (DishID) REFERENCES "Dish",
    CONSTRAINT FK_DishIngredientsRelation_IngID FOREIGN KEY (IngredientID) REFERENCES "Ingredient"
);

CREATE TABLE "DrinkIngredientsRelation" (
    DrinkID INT NOT NULL,
    IngredientID INT NOT NULL,

    CONSTRAINT PK_DrinkIngredientsRelation PRIMARY KEY (DrinkID, IngredientID),
    CONSTRAINT FK_DrinkIngredientsRelation_DrinkID FOREIGN KEY (DrinkID) REFERENCES "Drink",
    CONSTRAINT FK_DrinkIngredientsRelation_IngID FOREIGN KEY (IngredientID) REFERENCES "Ingredient"
);

CREATE TABLE "Reservation" (
    ID INT NOT NULL PRIMARY KEY,
    NumberOfRooms INT NOT NULL,
    TimeOfArrival TIMESTAMP NOT NULL,
    CreatedBy INT NOT NULL,

    CONSTRAINT FK_ReservationCreateBy FOREIGN KEY (CreatedBy) REFERENCES "User"
);

CREATE TABLE "RoomType" (
    ID INT NOT NULL PRIMARY KEY,
    Name VARCHAR(256)
);

CREATE TABLE "Room" (
    ID INT NOT NULL PRIMARY KEY,
    NumberOfSeats INT,
    RoomType INT,
    Availability INT DEFAULT 1 NOT NULL,

    CONSTRAINT AvailableStatus CHECK ( Availability >= 0 AND Availability <= 1 ),
    CONSTRAINT FK_RoomType FOREIGN KEY (RoomType) REFERENCES "RoomType"
);

CREATE TABLE "ReservationRoomRelation" (
    ReservationID INT NOT NULL,
    RoomID INT NOT NULL,

    CONSTRAINT PK_RRR PRIMARY KEY (ReservationID, RoomID),
    CONSTRAINT FK_RRR_Reservation FOREIGN KEY (ReservationID) REFERENCES "Reservation",
    CONSTRAINT FK_RRR_Room FOREIGN KEY (RoomID) REFERENCES "Room"
);

CREATE TABLE "Patch" (
    RoomID INT NOT NULL,
    PatchID INT NOT NULL,
    Herbs VARCHAR(256),
    Vegetables VARCHAR(256),

    CONSTRAINT PK_Patch PRIMARY KEY (RoomID, PatchID),
    CONSTRAINT FK_Patch_RoomID FOREIGN KEY (RoomID) REFERENCES "Room"
);

-- TRIGGERS --

-- This trigger toggles availability flag once new reservation is created.
-- Insertion is aborted when room is unavailable
-- After reservation is removed, availability is set back to 1 (available)
CREATE OR REPLACE TRIGGER "SetAvailability"
    BEFORE INSERT OR DELETE ON "ReservationRoomRelation"
    FOR EACH ROW
DECLARE
    available "Room".Availability%TYPE;
    room_unavailable EXCEPTION;
BEGIN
    -- cannot insert reservation on a room that is not available
    IF INSERTING THEN
        SELECT Availability into available FROM "Room" WHERE "Room".ID = :new.RoomID;
        IF available <> 1 THEN
            RAISE room_unavailable;
        END IF;
        UPDATE "Room"
        SET Availability = 0
        WHERE ID = :new.RoomID;
    ELSIF DELETING THEN
        UPDATE "Room"
        SET Availability = 1
        WHERE ID = :old.RoomID;
    END IF;
EXCEPTION
    WHEN room_unavailable THEN
        RAISE_APPLICATION_ERROR(-20000, 'Room is unavailable');
END;
/

-- Update order price base on the dish ordered
CREATE OR REPLACE TRIGGER "PaymentPriceUpdateDish"
    AFTER INSERT OR DELETE ON "AmountOfItemsDish"
    FOR EACH ROW
DECLARE
    PriceOfItem "Payment".Price%TYPE DEFAULT 0;
BEGIN
    IF INSERTING THEN
        SELECT D.PricePerItem * :new.ItemCount INTO PriceOfItem
            FROM "Dish" D WHERE D.ID = :new.DishID;
        UPDATE "Payment"
        SET Price = Price + PriceOfItem
        WHERE ID = (SELECT ContainsPayment
                    FROM "Order"
                    WHERE "Order".ID = :new.OrderID);
    ELSIF DELETING THEN
        SELECT D.PricePerItem * :old.ItemCount INTO PriceOfItem
        FROM "Dish" D WHERE D.ID = :old.DishID;
        UPDATE "Payment"
        SET Price = Price - PriceOfItem
        WHERE ID = (SELECT ContainsPayment
                    FROM "Order"
                    WHERE "Order".ID = :old.OrderID);
    END IF;
END;
/

-- END OF TRIGGERS --

-- User
INSERT INTO "User" (ID, EMAIL, PHONENUM, NAME, SURNAME) VALUES (0, 'heires_irric@email.to', '249 173 598', 'Heires', 'Irric');
INSERT INTO "User" (ID, EMAIL, PHONENUM, NAME, SURNAME) VALUES (1, 'laia_callum@email.to', '+420812130963', 'Laia', 'Callum');
INSERT INTO "User" (ID, EMAIL, PHONENUM, NAME, SURNAME) VALUES (2, 'gerrin_kossia@email.to', '+420 709 371 001', 'Gerrin', 'Kossia');
INSERT INTO "User" (ID, EMAIL, PHONENUM, NAME, SURNAME) VALUES (3, 'palisia_oakina@email.to', '00420663253965', 'Palisia', 'Oakina');
INSERT INTO "User" (ID, EMAIL, PHONENUM, NAME, SURNAME) VALUES (4, 'fysafa_kerina@email.to', '00420 554 035 130', 'Fysafa', 'Kerina');
INSERT INTO "User" (ID, EMAIL, PHONENUM, NAME, SURNAME) VALUES (5, 'zale_shardi@email.to', '889642937', 'Zale', 'Shardi');
INSERT INTO "User" (ID, EMAIL, PHONENUM, NAME, SURNAME) VALUES (6, 'heires_bafon@email.to', '717875198', 'Heires', 'Bafon');

-- Employee
INSERT INTO "Employee" (ID, DATEOFBIRTH, CONTACTTYPE, SUPERIOR, PARENTTABLE) VALUES (0, TO_DATE('1970-11-21', 'YYYY-MM-DD'), 0, NULL, 0);
INSERT INTO "Employee" (ID, DATEOFBIRTH, CONTACTTYPE, SUPERIOR, PARENTTABLE) VALUES (1, TO_DATE('1983-1-3', 'YYYY-MM-DD'), 0, 0, 2);
INSERT INTO "Employee" (ID, DATEOFBIRTH, CONTACTTYPE, SUPERIOR, PARENTTABLE) VALUES (2, TO_DATE('1978-12-31', 'YYYY-MM-DD'), 1, 0, 1);
INSERT INTO "Employee" (ID, DATEOFBIRTH, CONTACTTYPE, SUPERIOR, PARENTTABLE) VALUES (3, TO_DATE('1978-12-31', 'YYYY-MM-DD'), 1, NULL, 3);

-- Payment
INSERT INTO "Payment" (ID, DATETIME, PAYMENTTYPE, CREATEDBY) VALUES (0, CURRENT_TIMESTAMP, 0, 0);
INSERT INTO "Payment" (ID, DATETIME, PAYMENTTYPE, CREATEDBY) VALUES (1, CURRENT_TIMESTAMP, 1, 2);
INSERT INTO "Payment" (ID, DATETIME, PAYMENTTYPE, CREATEDBY) VALUES (2, CURRENT_TIMESTAMP, 1, 0);
INSERT INTO "Payment" (ID, DATETIME, PAYMENTTYPE, CREATEDBY) VALUES (3, CURRENT_TIMESTAMP, 0, 3);

-- Order
INSERT INTO "Order" (ID, ORDERSTATUS, DATETIME, CONTAINSPAYMENT, CREATEDBY, ORDEREDBY) VALUES (0, 0, CURRENT_TIMESTAMP, 2, 0, 5);
INSERT INTO "Order" (ID, ORDERSTATUS, DATETIME, CONTAINSPAYMENT, CREATEDBY, ORDEREDBY) VALUES (1, 1, CURRENT_TIMESTAMP, 1, 2, 6);
INSERT INTO "Order" (ID, ORDERSTATUS, DATETIME, CONTAINSPAYMENT, CREATEDBY, ORDEREDBY) VALUES (2, 1, CURRENT_TIMESTAMP, 0, 2, 4);
INSERT INTO "Order" (ID, ORDERSTATUS, DATETIME, CONTAINSPAYMENT, CREATEDBY, ORDEREDBY) VALUES (3, 1, CURRENT_TIMESTAMP, 3, 0, 5);
INSERT INTO "Order" (ID, ORDERSTATUS, DATETIME, CONTAINSPAYMENT, CREATEDBY, ORDEREDBY) VALUES (4, 1, CURRENT_TIMESTAMP, 3, 0, 5);
INSERT INTO "Order" (ID, ORDERSTATUS, DATETIME, CONTAINSPAYMENT, CREATEDBY, ORDEREDBY) VALUES (5, 0, CURRENT_TIMESTAMP, 2, 1, 4);

-- Dish
INSERT INTO "Dish" (ID, NAME, PRICEPERITEM, AMOUNTINSTOCK, WEIGHT, PORTIONSIZE) VALUES (0, 'Coconut Rice', 119.99, 5, 200, 1);
INSERT INTO "Dish" (ID, NAME, PRICEPERITEM, AMOUNTINSTOCK, WEIGHT, PORTIONSIZE) VALUES (1, 'Mango and Eggplant Noodles', 169.99, 3, 350, 1);
INSERT INTO "Dish" (ID, NAME, PRICEPERITEM, AMOUNTINSTOCK, WEIGHT, PORTIONSIZE) VALUES (2, 'Thai Red Curry with Tofu', 119.99, 10, 150, 0);
INSERT INTO "Dish" (ID, NAME, PRICEPERITEM, AMOUNTINSTOCK, WEIGHT, PORTIONSIZE) VALUES (3, 'Baklava', 69.99, 15, 100, 0);
INSERT INTO "Dish" (ID, Name, PricePerItem, AmountInStock, Weight, PortionSize) VALUES (4, 'Fried Rice', 59.99, 15, 100, 1);
INSERT INTO "Dish" (ID, Name, PricePerItem, AmountInStock, Weight, PortionSize) VALUES (5, 'Vegan Mapo Tofu', 134.99, 5, 200, 1);
INSERT INTO "Dish" (ID, Name, PricePerItem, AmountInStock, Weight, PortionSize) VALUES (6, 'Tzatziki', 39.99, 10, 250, 0);
INSERT INTO "Dish" (ID, Name, PricePerItem, AmountInStock, Weight, PortionSize) VALUES (7, 'Grilled Vegetables', 79.99, 15, 175, 0);
INSERT INTO "Dish" (ID, Name, PricePerItem, AmountInStock, Weight, PortionSize) VALUES (8, 'Mango curry', 219.99, 7, 300, 1);
INSERT INTO "Dish" (ID, Name, PricePerItem, AmountInStock, Weight, PortionSize) VALUES (9, 'Vegan Pho with Seitan', 249.99, 7, 300, 1);
INSERT INTO "Dish" (ID, Name, PricePerItem, AmountInStock, Weight, PortionSize) VALUES (10, 'Fried Noodles with Veggies', 119.99, 7, 200, 1);


-- Drink
INSERT INTO "Drink" (ID, NAME, PRICEPERITEM, AMOUNTINSTOCK, VOLUME) VALUES (0, 'Mango Lassi', 39.99, 15, 250);
INSERT INTO "Drink" (ID, NAME, PRICEPERITEM, AMOUNTINSTOCK, VOLUME) VALUES (1, 'Masala Tea', 54.99, 20, 200);
INSERT INTO "Drink" (ID, NAME, PRICEPERITEM, AMOUNTINSTOCK, VOLUME) VALUES (2, 'Bernard BPA', 49.99, 35, 500);
INSERT INTO "Drink" (ID, NAME, PRICEPERITEM, AMOUNTINSTOCK, VOLUME) VALUES (3, 'Capri Sun', 29.99, 10, 300);

-- Ingredient
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (0, 'Eggplant', CURRENT_DATE, 5);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (1, 'Tofu', CURRENT_DATE, 8);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (2, 'Rice', CURRENT_DATE, 13);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (3, 'Milk', CURRENT_DATE, 17);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (4, 'Yogurt', CURRENT_DATE, 21);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (5, 'Mango', CURRENT_DATE, 56);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (6, 'Bell Pepper', CURRENT_DATE, 12);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (7, 'Carrot', CURRENT_DATE, 27);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (8, 'Seitan', CURRENT_DATE, 18);
INSERT INTO "Ingredient" (ID, Name, DateOfDelivery, AmountInStock) VALUES (9, 'Flour', CURRENT_DATE, 33);


-- DishIngredientsRelation
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (0, 2);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (1, 5);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (2, 1);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (4, 2);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (5, 1);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (5, 8);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (6, 4);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (6, 3);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (7, 0);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (7, 7);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (7, 6);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (8, 5);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (8, 8);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (9, 8);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (9, 7);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (9, 6);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (10, 6);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (10, 7);
INSERT INTO "DishIngredientsRelation" (DishID, IngredientID) VALUES (10, 9);




-- DrinkIngredientsRelation
INSERT INTO "DrinkIngredientsRelation" (DrinkID, IngredientID) VALUES (0, 5);
INSERT INTO "DrinkIngredientsRelation" (DrinkID, IngredientID) VALUES (1, 3);
INSERT INTO "DrinkIngredientsRelation" (DrinkID, IngredientID) VALUES (0, 4);

-- Allergens
INSERT INTO "Allergens" (NAME) VALUES ('Milk');
INSERT INTO "Allergens" (NAME) VALUES ('Soybeans');
INSERT INTO "Allergens" (NAME) VALUES ('Nuts');
INSERT INTO "Allergens" (NAME) VALUES ('Gluten');

-- AllergensDishRelation
INSERT INTO "AllergensDishRelation" (DishID, AllergenID) VALUES (2, 'Soybeans');
INSERT INTO "AllergensDishRelation" (DishID, AllergenID) VALUES (3, 'Nuts');
INSERT INTO "AllergensDishRelation" (DishID, AllergenID) VALUES (1, 'Gluten');
INSERT INTO "AllergensDishRelation" (DishID, AllergenID) VALUES (6, 'Milk');
INSERT INTO "AllergensDishRelation" (DishID, AllergenID) VALUES (8, 'Milk');
INSERT INTO "AllergensDishRelation" (DishID, AllergenID) VALUES (9, 'Gluten');
INSERT INTO "AllergensDishRelation" (DishID, AllergenID) VALUES (5, 'Soybeans');

-- AllergensDrinkRelation
INSERT INTO "AllergensDrinkRelation" (DrinkID, AllergenID) VALUES (0, 'Milk');

-- Amount of items
INSERT INTO "AmountOfItemsDrink" (OrderID, DrinkID, ItemCount)VALUES (2, 0, 1);
INSERT INTO "AmountOfItemsDish" (OrderID, DishID, ItemCount) VALUES (3, 1, 1);
INSERT INTO "AmountOfItemsDrink" (OrderID, DrinkID, ItemCount)VALUES (0, 2, 1);
INSERT INTO "AmountOfItemsDrink" (OrderID, DrinkID, ItemCount)VALUES (1, 2, 3);
INSERT INTO "AmountOfItemsDish" (OrderID, DishID, ItemCount) VALUES (1, 2, 2);
INSERT INTO "AmountOfItemsDish" (OrderID, DishID, ItemCount) VALUES (1, 0, 2);
INSERT INTO "AmountOfItemsDish" (OrderID, DishID, ItemCount) VALUES (5, 4, 3);

INSERT INTO "RoomType" (ID, Name) VALUES (1, 'Garden');
INSERT INTO "RoomType" (ID, Name) VALUES (2, 'Balcony');
INSERT INTO "RoomType" (ID, Name) VALUES (3, 'VIP');
INSERT INTO "RoomType" (ID, Name) VALUES (4, 'Common');
INSERT INTO "RoomType" (ID, Name) VALUES (5, 'Near Window');

-- VIP
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (0, 2, 3);
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (1, 2, 3);
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (2, 2, 3);
-- Garden
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (3, 6, 1);
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (4, 6, 1);
-- Common
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (5, 4, 4);
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (6, 4, 4);
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (7, 4, 4);
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (8, 4, 4);
 -- Balcony
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (9, 4, 2);
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (10, 4, 2);
-- Near Window
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (11, 5, 2);
INSERT INTO "Room" (ID, NumberOfSeats, RoomType) VALUES (12, 5, 2);

-- Reservation
INSERT INTO "Reservation" (ID, NumberOfRooms, TimeOfArrival, CreatedBy) VALUES (0, 2, CURRENT_TIMESTAMP, 0);
INSERT INTO "Reservation" (ID, NumberOfRooms, TimeOfArrival, CreatedBy) VALUES (1, 1, CURRENT_TIMESTAMP, 1);

-- ReservationRoomRelation
INSERT INTO "ReservationRoomRelation" (ReservationID, RoomID) VALUES (0, 3);
INSERT INTO "ReservationRoomRelation" (ReservationID, RoomID) VALUES (0, 4);
INSERT INTO "ReservationRoomRelation" (ReservationID, RoomID) VALUES (1, 11);

-- Patch
INSERT INTO "Patch" (RoomID, PatchID, Herbs, Vegetables) VALUES (3, 0, 'Rosemary', NULL);
INSERT INTO "Patch" (RoomID, PatchID, Herbs, Vegetables) VALUES (4, 1, 'Thyme', NULL);
INSERT INTO "Patch" (RoomID, PatchID, Herbs, Vegetables) VALUES (11, 2, 'Parsley', NULL);
INSERT INTO "Patch" (RoomID, PatchID, Herbs, Vegetables) VALUES (12, 3, 'Oregano', 'Tomato');
INSERT INTO "Patch" (RoomID, PatchID, Herbs, Vegetables) VALUES (3, 4, 'Rosemary', NULL);
INSERT INTO "Patch" (RoomID, PatchID, Herbs, Vegetables) VALUES (12, 5, 'Parsley', NULL);
INSERT INTO "Patch" (RoomID, PatchID, Herbs, Vegetables) VALUES (10, 6, 'Parsley', 'Eggplant');

-- END OF TASK 1

-- TASK 3 SELECT QUERIES
-- U každého z dotazů musí být (v komentáři SQL kódu)
-- popsáno srozumitelně, jaká data hledá daný dotaz (jaká je jeho funkce v aplikaci).

-- dva dotazy využívající spojení dvou tabulek:
-- 1. Kteri zakaznici maji objednavku ve stavu 1 ?
SELECT "User".ID, NAME, SURNAME, "Order".ID
FROM "User" JOIN "Order" ON "User".ID = "Order".OrderedBy
WHERE "Order".OrderStatus=1;

-- 2. Kteri zamestanci vytvorili platbu s vetsi hodnotou nez 100?
SELECT E.ID, P.Price
FROM "Employee" E, "Payment" P
WHERE E.ID = P.CreatedBy AND P.Price > 100;

-- jeden využívající spojení tří tabulek:
-- 1. Jaké ingredience jsou obsaženy v jídle "Coconut rice"?
SELECT DISTINCT "Ingredient".Name
FROM "Ingredient" JOIN "DishIngredientsRelation" ON "Ingredient".ID = "DishIngredientsRelation".IngredientID
                  JOIN "Dish" ON "DishIngredientsRelation".DishID = "Dish".ID
WHERE "Dish".Name='Coconut Rice';

-- dva dotazy s klauzulí GROUP BY a agregační funkcí:
-- 1. Kolik mistnosti si dohromady kazdy zakaznik rezervoval?
SELECT "User".ID, "User".Name, COUNT("Reservation".ID) "Pocet mistnosti"
FROM "User" LEFT JOIN "Reservation" ON "User".ID = "Reservation".CreatedBy
GROUP BY "User".ID, "User".Name;

-- 2. Jaká jsou maximální částky plateb, které jednotliví zaměstnanci vytvořili?
SELECT E.ID, MAX(P.Price)
FROM "Employee" E, "Payment" P
WHERE E.ID = P.CreatedBy
GROUP BY E.ID;

-- jeden dotaz obsahující predikát EXISTS:
-- Ktere mistnosti mají zahonky pouze s bylinkou "Parsley" a libovolnou (i žádnou) zeleninou?
SELECT DISTINCT R.ID, P.PatchID, P.Herbs, P.Vegetables
FROM "Room" R, "Patch" P
WHERE R.ID = P.ROOMID
  AND P.HERBS = 'Parsley'
  AND NOT EXISTS (SELECT *
                  FROM "Patch" P
                  WHERE R.ID = P.ROOMID AND
                          P.HERBS <> 'Parsley');

-- jeden dotaz s predikátem IN s vnořeným selectem (nikoliv IN s množinou konstantních dat):
-- Jaci zakaznici si objednali jidlo s ryzi, kdy a jaky pokrm to byl?
SELECT "User".Name, "User".Surname, "Dish".Name, "Order".DateTime
FROM "User" JOIN "Order" ON "User".ID = "Order".OrderedBy JOIN "AmountOfItemsDish" ON "Order".ID = "AmountOfItemsDish".OrderID
            JOIN "Dish" ON "AmountOfItemsDish".DishID = "Dish".ID
WHERE "Dish".ID in
      (SELECT "Dish".ID
       FROM "Dish" JOIN "DishIngredientsRelation" ON "Dish".ID = "DishIngredientsRelation".DishID JOIN "Ingredient" ON "DishIngredientsRelation".IngredientID = "Ingredient".ID
       WHERE "Ingredient".Name = 'Rice')
ORDER BY "Order".DateTime;

-- END OF TASK 3
-- TASK 4

-- TEST OF TRIGGERS (line 216)
-- Three rooms should show up
SELECT ID FROM "Room" WHERE "Room".Availability = 0;
DELETE FROM "ReservationRoomRelation" WHERE RoomID=4;
-- Now only two rooms should show up
SELECT ID FROM "Room" WHERE "Room".Availability = 0;

-- The total price should equal to 0
SELECT ID, Price FROM "Payment" WHERE ID=0;
-- insert new item into order 2 which is associated with payment 0
INSERT INTO "AmountOfItemsDish" (OrderID, DishID, ItemCount) VALUES (2, 0, 2);
-- total price should now update
SELECT ID, Price FROM "Payment" WHERE ID=0;

-- 2. Procedury
-- a) Procedura na vypsani uctenky.
CREATE OR REPLACE PROCEDURE "printReceipt" (paymentID IN INT)
AS
    totalPrice FLOAT;
    CURSOR dishItems IS SELECT "Dish".Name, "Dish".PricePerItem, "AmountOfItemsDish".ItemCount FROM "Dish", "AmountOfItemsDish", "Order"
        WHERE "Order".ContainsPayment = paymentID AND "Order".ID = "AmountOfItemsDish".OrderID AND
              "AmountOfItemsDish".DishID = "Dish".ID;

    CURSOR drinkItems IS SELECT "Drink".Name, "Drink".PricePerItem, "AmountOfItemsDrink".ItemCount FROM "Drink", "AmountOfItemsDrink", "Order"
    WHERE "Order".ContainsPayment = paymentID AND "Order".ID = "AmountOfItemsDrink".OrderID AND
          "AmountOfItemsDrink".DrinkID = "Drink".ID;
    dishName VARCHAR(256);
    dishPrice FLOAT;
    dishCount INT;

BEGIN
    totalPrice := 0;
    DBMS_OUTPUT.put_line(
    'Items ordered: '
    );
    OPEN dishItems;
    LOOP
        FETCH dishItems INTO dishName, dishPrice, dishCount;
        EXIT WHEN dishItems%NOTFOUND;

        totalPrice := totalPrice + (dishPrice*dishCount);
        DBMS_OUTPUT.put_line(
        dishName || ': Number of items: ' || dishCount || ', Subtotal cost: ' || dishPrice*dishCount || ' CZK'
        );

    END LOOP;

    OPEN drinkItems;
    LOOP
        FETCH drinkItems INTO dishName, dishPrice, dishCount;
        EXIT WHEN drinkItems%NOTFOUND;

        totalPrice := totalPrice + (dishPrice*dishCount);
        DBMS_OUTPUT.put_line(
        dishName || ': Number of items: ' || dishCount || ', Subtotal cost: ' || dishPrice*dishCount || ' CZK'
        );

    END LOOP;

    DBMS_OUTPUT.put_line(
    'Total cost: ' || totalPrice || ' CZK'
    );

END;
-- Predvedeni procedury na vypsani uctenky.
BEGIN "printReceipt"(1);
END;

-- b) Procedura na vypsani menu.
CREATE OR REPLACE PROCEDURE "printMenu"
AS
    totalPrice FLOAT;
    CURSOR dishItems IS SELECT Name, PricePerItem, PortionSize, Weight FROM "Dish";
    CURSOR drinkItems IS SELECT Name, PricePerItem, Volume FROM "Drink";
    name VARCHAR(256);
    price FLOAT;
    portionsize INT;
    weight INT;
    volume INT;

BEGIN
    totalPrice := 0;
    DBMS_OUTPUT.put_line(
    'Menu: '
    );
    DBMS_OUTPUT.put_line(
    'Dishes: '
    );
    OPEN dishItems;
    LOOP
        FETCH dishItems INTO name, price, portionsize, weight;
        EXIT WHEN dishItems%NOTFOUND;

        IF portionsize = 1
        THEN
        DBMS_OUTPUT.put_line(
        name || ' (' || weight || ' g) Price: ' || price || ' CZK'
        );
        END IF;

        IF portionsize = 0
        THEN
        DBMS_OUTPUT.put_line(
        name || ' (' || weight || ' g) Price: ' || price || ' CZK (Kids portion)'
        );
        END IF;
    END LOOP;
    DBMS_OUTPUT.put_line(
    'Drinks: '
    );
    OPEN drinkItems;
    LOOP
        FETCH drinkItems INTO name, price, volume;
        EXIT WHEN drinkItems%NOTFOUND;


        DBMS_OUTPUT.put_line(
        name || ' (' || volume || ' ml) Price: ' || price || ' CZK'
        );
    END LOOP;
END;
-- Predvedeni procedury na vypsani menu
BEGIN "printMenu"();
END;

-- MATERIALIZED VIEW
-- Displays list of employees that has created at least one order
-- and with that how many did they created
CREATE MATERIALIZED VIEW "OrderInsertionCounter" REFRESH ON COMMIT AS
SELECT E.ID, U.Name, U.Surname, COUNT(O.ID) NumberOfOrders
FROM "Employee" E, "Order" O, "User" U
WHERE E.ID = O.CreatedBy AND E.PARENTTABLE = U.ID
GROUP BY U.Name, E.ID, U.Surname;

-- TEST VIEW
-- Three rows should be displayed
SELECT * FROM "OrderInsertionCounter";
-- If user doesn't commit this update
-- nothing will change
UPDATE "Order" SET "Order".CreatedBy = 3 WHERE ID = 5;
-- This table should be identical with table from line 424
-- if user didn't commit update
SELECT * FROM "OrderInsertionCounter";


-- EXPLAIN PLAN --
-- In how many dishes are these ingredients used in?
-- Show me only ingredients with more than 20 pieces in stock
EXPLAIN PLAN FOR
SELECT I.Name, Count(D.ID) NuberOfDishes
FROM "DishIngredientsRelation" JOIN "Dish" D on D.ID = "DishIngredientsRelation".DishID
    RIGHT JOIN "Ingredient" I ON I.ID = "DishIngredientsRelation".IngredientID
WHERE I.AmountInStock > 20
GROUP BY I.Name
ORDER BY I.Name;
-- first input
SELECT  * FROM TABLE ( DBMS_XPLAN.DISPLAY );

-- Optimize table by clustering ingredients by amount in stock
CREATE INDEX "index" ON "Ingredient" (AmountInStock);

EXPLAIN PLAN FOR
SELECT I.Name, Count(D.ID) NuberOfDishes
FROM "DishIngredientsRelation" JOIN "Dish" D on D.ID = "DishIngredientsRelation".DishID
                               RIGHT JOIN "Ingredient" I ON I.ID = "DishIngredientsRelation".IngredientID
WHERE I.AmountInStock > 20
GROUP BY I.Name
ORDER BY I.Name;
-- second input
SELECT  * FROM TABLE ( DBMS_XPLAN.DISPLAY );


-- 4. Pristupova prava pro druheho clena tymu
GRANT ALL ON "Allergens" TO "XDOHUN00";
GRANT ALL ON "AmountOfItemsDish" TO "XDOHUN00";
GRANT ALL ON "AmountOfItemsDrink" TO "XDOHUN00";
GRANT ALL ON "User" TO "XDOHUN00";
GRANT ALL ON "Dish" TO "XDOHUN00";
GRANT ALL ON "DrinkIngredientsRelation" TO "XDOHUN00";
GRANT ALL ON "DishIngredientsRelation" TO "XDOHUN00";
GRANT ALL ON "Drink" TO "XDOHUN00";
GRANT ALL ON "AllergensDishRelation" TO "XDOHUN00";
GRANT ALL ON "AllergensDrinkRelation" TO "XDOHUN00";
GRANT ALL ON "Employee" TO "XDOHUN00";
GRANT ALL ON "Ingredient" TO "XDOHUN00";
GRANT ALL ON "Order" TO "XDOHUN00";
GRANT ALL ON "Patch" TO "XDOHUN00";
GRANT ALL ON "Payment" TO "XDOHUN00";
GRANT ALL ON "Reservation" TO "XDOHUN00";
GRANT ALL ON "ReservationRoomRelation" TO "XDOHUN00";
GRANT ALL ON "Room" TO "XDOHUN00";
GRANT ALL ON "RoomType" TO "XDOHUN00";
GRANT ALL ON "OrderInsertionCounter" TO "XDOHUN00";
-- END OF TASK 4
-- end of xdohna48_xdohun00.sql
