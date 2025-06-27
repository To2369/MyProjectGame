#include "Collision.h"
#include <algorithm>
using namespace DirectX;
// 球と球の交差判定
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outVec
)
{
    DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
    // ２点間の位置ベクトル引く
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(posB, posA);
    // 単一の長さを計算
    DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
    // float 型に変換
    float len;
    DirectX::XMStoreFloat(&len, length);

    // 距離判定
    float range = radiusA + radiusB;
    if (len > range)
    {
        return false;
    }

    // 衝突していたら衝突方向のベクトルを設定
    vec = DirectX::XMVector3Normalize(vec);
    DirectX::XMStoreFloat3(&outVec, vec);

    return true;
}

// 円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A の足もとが B の頭より上なら衝突していない
    if (positionA.y > positionB.y + heightB)
        return false;

    // A の頭が B の足もとより下なら衝突していない
    if (positionA.y + heightA < positionB.y)
        return false;

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // 衝突方向のベクトル
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}

bool Collision::IntersectCapsuleVsCapsule(
    const DirectX::XMVECTOR& position1,	// 中心
    const DirectX::XMVECTOR& direction1,	// 向き（正規化）
    const float					length1,	// 長さ
    const float					radius1,	// 半径
    const DirectX::XMVECTOR& position2,	// 中心
    const DirectX::XMVECTOR& direction2,	// 向き（正規化）
    const float					length2,	// 長さ
    const float					radius2,	// 半径
    IntersectionResult* result)
{


    // 各カプセルの中心線上の最近点算出
    //向きの方向にしか伸びてないからこれはだめっだった
    DirectX::XMVECTOR point1 = {}, point2 = {};
    DirectX::XMVECTOR end1 = DirectX::XMVectorAdd(position1, DirectX::XMVectorScale(direction1, length1));
    DirectX::XMVECTOR end2 = DirectX::XMVectorAdd(position2, DirectX::XMVectorScale(direction2, length2));

    // 線分同士の最短距離の二乗を取得
    float distSq = GetMinDistSq_SegmentSegment(position1, end1, position2, end2, &point1, &point2);
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);
    float radiusAdd = radius1 + radius2;

    // 最近接点間の距離がカプセルの半径の和より小さい場合、衝突している
    if (distSq < radiusAdd * radiusAdd)
    {
        if (result)
        {
            result->normal = DirectX::XMVector3Normalize(vec);
            result->penetration = radiusAdd - sqrtf(distSq);
            result->pointA = DirectX::XMVectorAdd(point1, DirectX::XMVectorScale(result->normal, -radius1));
            result->pointB = DirectX::XMVectorAdd(point2, DirectX::XMVectorScale(result->normal, radius2));
        }

        return true;
    }

    return false;
}

// 線分と線分の最短距離の二乗を取得する
float Collision::GetMinDistSq_SegmentSegment(
    const  DirectX::XMVECTOR& point1A,
    const  DirectX::XMVECTOR& point1B,
    const  DirectX::XMVECTOR& point2A,
    const  DirectX::XMVECTOR& point2B,
    DirectX::XMVECTOR* nearPoint1,
    DirectX::XMVECTOR* nearPoint2)
{
    DirectX::XMVECTOR segmentDirection1 = DirectX::XMVectorSubtract(point1B, point1A);
    DirectX::XMVECTOR segmentDirection2 = DirectX::XMVectorSubtract(point2B, point2A);
    DirectX::XMVECTOR r = DirectX::XMVectorSubtract(point1A, point2A);

    float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection1));
    float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, segmentDirection2));
    float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, r));

    float t1 = 0.0f, t2 = 0.0f;

    if (a <= FLT_EPSILON && e <= FLT_EPSILON)	// 両線分が点に縮退している場合
    {
        t1 = t2 = 0.0f;
    }
    else if (a <= FLT_EPSILON)					// 片方（d0）が点に縮退している場合
    {
        t1 = 0.0f;
        t2 = std::clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, r));
        if (e <= FLT_EPSILON)					// 片方（d1）が点に縮退している場合
        {
            t2 = 0.0f;
            t1 = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else									// 両方が線分
        {
            float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection2));
            float denom = a * e - b * b;

            if (denom != 0.0f)					// 平行確認（平行時は t1 = 0.0f（線分の始端）を仮の初期値として計算をすすめる）
            {
                t1 = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }

            t2 = b * t1 + f;

            if (t2 < 0.0f)						// t1が始端より外側にある場合
            {
                t2 = 0.0f;
                t1 = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t2 > e)					// t1が終端より外側にある場合
            {
                t2 = 1.0f;
                t1 = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
            else								// t1が線分上にある場合
            {
                t2 /= e;
            }
        }
    }

    // 各線分上の最近点算出
    DirectX::XMVECTOR point1 = DirectX::XMVectorAdd(point1A, DirectX::XMVectorScale(segmentDirection1, t1));
    DirectX::XMVECTOR point2 = DirectX::XMVectorAdd(point2A, DirectX::XMVectorScale(segmentDirection2, t2));

    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);

    if (nearPoint1)
    {
        *nearPoint1 = point1;
    }
    if (nearPoint2)
    {
        *nearPoint2 = point2;
    }

    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));
}

// 球と円柱の交差反映
bool Collision::IntersectSphereVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A（球）の足もとが B（円柱）の頭より上なら衝突していない
    if (positionA.y - radiusA > positionB.y + heightB)
        return false;

    // A（球）の頭が B（円柱）の足もとより下なら衝突していない
    if (positionA.y + radiusA < positionB.y)
        return false;

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // 衝突方向のベクトル
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}

inline DirectX::XMFLOAT3 GetTriangleNormVector(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
    //①三角形trの法線nを求める
    //※３点が時計回りに見える側を表とする
    DirectX::XMFLOAT3 n = {};

    //p1-p0のベクトル
    DirectX::XMVECTOR a = DirectX::XMVectorSubtract(
        DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));
    //p2-p1のベクトル
    DirectX::XMVECTOR b = DirectX::XMVectorSubtract(
        DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p1)
    );
    //上記の2辺の外積の正規化された値
    DirectX::XMStoreFloat3(&n, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(a, b)));

    return n;
}

bool RayVsTriangle(DirectX::XMFLOAT3* p, const Ray& r, const Triangle& t, float& neart)
{
    //④光線rが三角形tの表側から交差し、かつ、交点pが三角形tの内部にある場合のみ、trueを返す
    DirectX::XMFLOAT3 n = GetTriangleNormVector(t.p0, t.p1, t.p2);



    //光線(レイ)の方向ベクトルr.dと面法線ベクトルnの内積fDotを計算します
    float fDot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&r.d), DirectX::XMLoadFloat3(&n)));

    // 内積がマイナスかどうか判定（表面ならを続ける）
    if (fDot < 0.0f)
    {
        // 光線が真横から当たっていないか判定（fabs(fDot) > 1e-6f）
        if (fabs(fDot) > 1e-6f)
        {
            // 光線と三角形の交点までの光線上の距離を計算（スケール値の計算）
            float l;
            DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p0), DirectX::XMLoadFloat3(&r.p));
            l = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, DirectX::XMLoadFloat3(&n))) / fDot;

            if (l > neart)
                return false;

            // 距離が０より大きいかどうか判定
            if (l > 0)
            {
                // 交点の座標位置を計算
                //*p = {r.p.x+r.d.x+l,r.p.y + r.d.y + l, r.p.z + r.d.z + l };
                DirectX::XMStoreFloat3(p,
                    DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&r.p),
                        DirectX::XMVectorScale(DirectX::XMLoadFloat3(&r.d), l)));
                //平面上にある点pを計算(始点＋方向ベクトル＊スケール)
                // 交点から三角形のそれぞれの点までのベクトルを計算
                DirectX::XMVECTOR tmp0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p0), DirectX::XMLoadFloat3(p));
                DirectX::XMVECTOR tmp1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p1), DirectX::XMLoadFloat3(p));
                DirectX::XMVECTOR tmp2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p2), DirectX::XMLoadFloat3(p));

                // 上記で計算したベクトルのそれぞれの外積を計算
                DirectX::XMVECTOR n0 = DirectX::XMVector3Cross(tmp0, tmp1);
                DirectX::XMVECTOR n1 = DirectX::XMVector3Cross(tmp1, tmp2);
                DirectX::XMVECTOR n2 = DirectX::XMVector3Cross(tmp2, tmp0);

                // 計算した外積の値と三角形の内積がすべて同じ方向を向いていたら（内積の値がプラスなら）
                if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(n0, DirectX::XMLoadFloat3(&n))) > 0 &&
                    DirectX::XMVectorGetX(DirectX::XMVector3Dot(n1, DirectX::XMLoadFloat3(&n))) > 0 &&
                    DirectX::XMVectorGetX(DirectX::XMVector3Dot(n2, DirectX::XMLoadFloat3(&n))) > 0)
                {
                    neart = l;
                    // 光線が三角形に交差している
                    return true;
                }
            }
        }
    }
    *p = {};
    return false;
}

// レイとモデルの交差判定
bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    const DirectX::XMFLOAT4X4& modelTransform,
    HitResult& result)
{
    // ワールド空間上でのレイの始点
    DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
    // ワールド空間上でのレイの終点
    DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
    // ワールド空間上でのレイの始点から終点までのベクトル
    DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
    // ワールド空間上でのレイの長さ
    DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
    DirectX::XMStoreFloat(&result.distance, worldRayLength);

    // true...衝突した
    bool hit = false;
    // メッシュごとに処理を行う
    for (const Model::Mesh& mesh : model->meshes)
    {

        // AABBの最小・最大点を取得
        DirectX::XMFLOAT3 meshMin = mesh.boundingBox[0];
        DirectX::XMFLOAT3 meshMax = mesh.boundingBox[1];

        // AABBの交差判定
        float tMin = 0.0f, tMax = 1.0f;
        AABB meshAABB = { meshMin, meshMax };
        if (!meshAABB.IntersectsRay(start, end, tMin, tMax))
        {
            continue;  // AABBとレイが交差しない場合、このメッシュをスキップ
        }

        // AABBと交差する場合に三角形との交差判定を行う
        //DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh_.defaultGlobalTransform);
        // メッシュ単体の transform
        DirectX::XMMATRIX worldTransformMat = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform), DirectX::XMLoadFloat4x4(&modelTransform));
        DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

        // ローカル空間でのレイの始点と終点を計算
        DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
        DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
        DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
        DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
        DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
        float localRayLength;
        DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);

        // 頂点データを取得
        const std::vector<Model::vertex>& vertices = mesh.vertices;
        const std::vector<UINT>& indices = mesh.indices;

        // 候補となる情報
        int materialIndex = -1;
        DirectX::XMVECTOR hitPosition;
        DirectX::XMVECTOR hitNormal;

        for (const Model::Mesh::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndexLocation + i;

                // 三角形の頂点の抽出
                const Model::vertex& a = vertices.at(indices.at(index));
                const Model::vertex& b = vertices.at(indices.at(index + 1));
                const Model::vertex& c = vertices.at(indices.at(index + 2));

                // レイと三角形の交点の座標（出力用）
                DirectX::XMFLOAT3 p;

                Ray r;
                DirectX::XMStoreFloat3(&r.p, localRayStartVec);
                DirectX::XMStoreFloat3(&r.d, localRayDirectVec);
                DirectX::XMStoreFloat(&r.l, localRayLengthVec);

                Triangle t;
                t.p0 = a.position;
                t.p1 = b.position;
                t.p2 = c.position;

                Sphere s;
                DirectX::XMStoreFloat3(&s.p, localRayStartVec);
                DirectX::XMStoreFloat3(&r.d, localRayDirectVec);
                DirectX::XMStoreFloat(&r.l, localRayLengthVec);
                s.r = 0;

                if (RayVsTriangle(&p, r, t, localRayLength))
                {
                    // 最近距離を更新
                    //localRayLength = length;

                    // マテリアル番号を更新
                    materialIndex = subset.materialUniqueID;

                    // 交点と法線を更新
                    hitPosition = DirectX::XMLoadFloat3(&p);
                    DirectX::XMFLOAT3 n = GetTriangleNormVector(t.p0, t.p1, t.p2);
                    hitNormal = DirectX::XMLoadFloat3(&n);
                }
            }
        }

        if (materialIndex >= 0)
        {
            // 交点座標をローカル空間からワールド空間へ変換
            DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
            // ワールド空間上でのレイの始点から交点までのベクトル
            DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
            // ワールド空間上でのレイの視点から交点までの長さ
            DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
            float distance;
            DirectX::XMStoreFloat(&distance, worldLengthVec);

            // ヒット結果情報保存
            if (result.distance > distance)
            {
                // ヒット時の面の法線をローカル空間からワールド空間へ変換
                DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(hitNormal, worldTransformMat);

                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, worldPositionVec);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
                hit = true;
            }
        }
    }

    return hit;
}

////bool Collision::IntersectSphereRayVsModel(
////    const DirectX::XMFLOAT3& start,
////    const DirectX::XMFLOAT3& end,
////    const float radius,
////    const Model* model,
////    HitResult& result)
////{
////    // ワールド空間上でのレイの始点
////    DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
////    // ワールド空間上でのレイの終点
////    DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
////    // ワールド空間上でのレイの始点から終点までのベクトル
////    DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
////    // ワールド空間上でのレイの長さ
////    DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
////    DirectX::XMStoreFloat(&result.distance, worldRayLength);
////
////    // true...衝突した
////    bool hit = false;
////
////    // メッシュごとに処理を行う
////    for (const Model::Mesh& mesh_ : model->meshes)
////    {
////        // AABBの最小・最大点を取得
////        DirectX::XMFLOAT3 meshMin = mesh_.boundingBox[0];
////        DirectX::XMFLOAT3 meshMax = mesh_.boundingBox[1];
////
////        // AABBの交差判定
////        float tMin = 0.0f, tMax = 1.0f;
////        AABB meshAABB = { meshMin, meshMax };
////        if (!meshAABB.IntersectsRay(start, end, tMin, tMax))
////        {
////            continue;  // AABBとレイが交差しない場合、このメッシュをスキップ
////        }
////
////        // AABBと交差する場合に三角形との交差判定を行う
////        DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh_.defaultGlobalTransform);
////        DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);
////
////        // ローカル空間でのレイの始点と終点を計算
////        DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
////        DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
////        DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
////        DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
////        DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
////        float localRayLength;
////        DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);
////
////        // 頂点データを取得
////        const std::vector<Model::vertex>& vertices = mesh_.vertices;
////        const std::vector<UINT>& indices = mesh_.indices;
////
////        // 候補となる情報
////        int materialIndex = -1;
////        DirectX::XMVECTOR hitPosition;
////        DirectX::XMVECTOR hitNormal;
////
////        for (const Model::Mesh::Subset& subset : mesh_.subsets)
////        {
////            for (UINT i = 0; i < subset.indexCount; i += 3)
////            {
////                UINT index = subset.startIndexLocation + i;
////
////                // 三角形の頂点の抽出
////                const Model::vertex& a = vertices.at(indices.at(index));
////                const Model::vertex& b = vertices.at(indices.at(index + 1));
////                const Model::vertex& c = vertices.at(indices.at(index + 2));
////
////                // レイと三角形の交点の座標（出力用）
////                DirectX::XMFLOAT3 p;
////
////                Ray r;
////                DirectX::XMStoreFloat3(&r.p, localRayStartVec);
////                DirectX::XMStoreFloat3(&r.d, localRayDirectVec);
////                DirectX::XMStoreFloat(&r.l, localRayLengthVec);
////
////                Triangle t;
////                t.p0 = a.position;
////                t.p1 = b.position;
////                t.p2 = c.position;
////
////                Sphere s;
////                DirectX::XMStoreFloat3(&s.p, localRayStartVec);
////
////                if (SphereCastVsTriangle(&p, r, t,s, localRayLength))
////                {
////                    // 最近距離を更新
////                    //localRayLength = length;
////
////                    // マテリアル番号を更新
////                    materialIndex = subset.materialUniqueID;
////
////                    // 交点と法線を更新
////                    hitPosition = DirectX::XMLoadFloat3(&p);
////                    DirectX::XMFLOAT3 n = GetTriangleNormVector(t.p0, t.p1, t.p2);
////                    hitNormal = DirectX::XMLoadFloat3(&n);
////                }
////            }
////        }
////
////        if (materialIndex >= 0)
////        {
////            // 交点座標をローカル空間からワールド空間へ変換
////            DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
////            // ワールド空間上でのレイの始点から交点までのベクトル
////            DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
////            // ワールド空間上でのレイの視点から交点までの長さ
////            DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
////            float distance;
////            DirectX::XMStoreFloat(&distance, worldLengthVec);
////
////            // ヒット結果情報保存
////            if (result.distance > distance)
////            {
////                // ヒット時の面の法線をローカル空間からワールド空間へ変換
////                DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(hitNormal, worldTransformMat);
////
////                result.distance = distance;
////                result.materialIndex = materialIndex;
////                DirectX::XMStoreFloat3(&result.position, worldPositionVec);
////                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
////                hit = true;
////            }
////        }
////    }
////
////    return hit;
////}